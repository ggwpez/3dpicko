/*
 * SPDX-FileCopyrightText: 2018-2020 Harald Gültig <hgueltig@gmail.com>
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

/*
Infos:
Größe Petrischale:
127,5 x 85
1,5:1
*/
//  Image        Colonies     Tooltips
var layer0 = {}, layer1 = {}, layer2 = {};

// Array of colonies. See ImageRecognition/include/Colony.hpp
var colony_array = [];
var number_of_colonies = 0;
let excluded_by_user = new Map(), included_by_user = new Map();
let excluded_colonies = new Map(), included_colonies = new Map();
let included_by_server;
let selected_colony_color;
var algorithms;
var changed_settings = { id: '', changed_algorithm: false, new_request: false, processed: true };

// fit size of canvas to window (max: image-size)
function resizeCanvas(img) {
    console.log("Offset width:", document.getElementById('detection-settings-div').offsetWidth);
    let width = $('#selection').width() - document.getElementById('detection-settings-div').offsetWidth - 20;
    let height = window.innerHeight - document.getElementById('layer_parent').getBoundingClientRect().top - 55;
    if (width < img.width || height < img.height) {
        down_scale = Math.min(width / img.width, height / img.height);
        width = down_scale * img.width;
        height = down_scale * img.height;
    }
    else {
        width = img.width;
        height = img.height;
        down_scale = 1;
    }

    console.log("Downscale factor: ", down_scale);

    layer0.canvas.width = width; // in pixels
    layer0.canvas.height = height; // in pixels

    layer1.canvas.width = width; // in pixels
    layer1.canvas.height = height; // in pixels

    layer2.canvas.width = width; // in pixels
    layer2.canvas.height = height; // in pixels

    layer0.context.drawImage(img, 0, 0, img.width, img.height, 0, 0, width, height);

    // fix width
    // TODO enable resizing?
    console.log("Offset width after resize:", document.getElementById('detection-settings-div').offsetWidth);
    console.log("width:", $('#detection-settings-div').width());
    $('#selection').css('min-width', $('#layer_parent').width() + 330);
}

function drawSafetyMargin(lt, rb) {
    let layer = document.getElementById('layer3').style;
    let topWidth = layer0.canvas.height * lt;
    let leftWidth = layer0.canvas.width * lt
    layer.borderTopWidth = topWidth + "px";
    layer.borderLeftWidth = leftWidth + "px";
    layer.borderRightWidth = Math.min(layer0.canvas.width - leftWidth, layer0.canvas.width * (1 - rb)) + "px";
    layer.borderBottomWidth = Math.min(layer0.canvas.height - topWidth, layer0.canvas.height * (1 - rb)) + "px";
}

let down_scale;
function selectionTabEnter() {
    layer0.canvas = document.getElementById('layer0');
    layer1.canvas = document.getElementById('layer1');
    layer2.canvas = document.getElementById('layer2');
    layer0.context = layer0.canvas.getContext('2d');
    layer1.context = layer1.canvas.getContext('2d');
    layer2.context = layer2.canvas.getContext('2d');

    let img = new Image();
    img.onload = function () {
        resizeCanvas(img);
        // show settings and start detection
        document.getElementById('layer-control').style.display = 'flex';
        document.getElementById('select-algorithm').onchange();
    }
    img.src = chosen_image.path;

    layer2.canvas.onmousemove = (e) => {
        var rect = layer2.canvas.getBoundingClientRect();
        var x = e.clientX - rect.left,
            y = e.clientY - rect.top;

        for (let colony of included_colonies.values()) {
            if (colony.isMouseOver(x, y)) {
                if (!colony.mouseover) {
                    colony.mouseover = true;
                    drawTooltip(colony);
                }
            }
            else if (colony.mouseover) {
                colony.mouseover = false;
                layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
            }
        }
        for (let colony of excluded_colonies.values()) {
            if (colony.isMouseOver(x, y)) {
                if (!colony.mouseover) {
                    colony.mouseover = true;
                    drawTooltip(colony);
                }
            }
            else if (colony.mouseover) {
                colony.mouseover = false;
                layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
            }
        }
    };

    layer2.canvas.onmousedown = (e) => {
        e.preventDefault();
        for (let colony of included_colonies.values()) {
            if (colony.mouseover) {
                colony.exclude();
                excluded_by_user.set(colony.get("hash"), colony);
                excluded_colonies.set(colony.get("hash"), colony);
                included_colonies.delete(colony.get("hash"));
                included_by_user.delete(colony.get("hash"));
                printPositions();
                drawTooltip(colony);
                return;
            }
        }
        for (let colony of excluded_colonies.values()) {
            if (colony.mouseover) {
                colony.include();
                included_by_user.set(colony.get("hash"), colony);
                included_colonies.set(colony.get("hash"), colony);
                excluded_colonies.delete(colony.get("hash"));
                excluded_by_user.delete(colony.get("hash"));
                printPositions();
                drawTooltip(colony);
                return;
            }
        }
    };

    layer2.canvas.onmouseleave = (e) => {
        // colony.mouseover = false ignored for performance...
        layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
    }

    let hide_colonies_button = document.getElementById('hide-colonies');
    document.getElementById('show-selected').onchange = function () {
        show_selected = this.checked;
        printPositions();
    };
    document.getElementById('show-excluded').onchange = function () {
        show_excluded = this.checked;
        printPositions();
    };
    document.getElementById('x-ray').onchange = function () {
        // TODO different settings if fluro is used...
        $('#layer0').toggleClass('x-ray', this.checked);
    };
    selected_colony_color = document.getElementById('selected-colony-color').value || "#ffffff";
    document.getElementById('selected-colony-color').onchange = function () {
        selected_colony_color = this.value;
        for (let colony of included_colonies.values()) {
            colony.set('linecolor', selected_colony_color);
        }
        for (let colony of included_by_user.values()) {
            colony.set('linecolor', selected_colony_color);
        }
        printPositions();
    };
}

function updatePositions(coords) {
    colony_array = coords.colonies;
    excluded_colonies = new Map();
    included_colonies = new Map();

    colony_array.forEach((colony, index) => {
        let hash = colony.x + ',' + colony.y;

        if (colony.excluded_by != "") {
            color = settings_color[changed_settings.id][colony.excluded_by] || 'grey';
            excluded_colonies.set(hash, new Circle({
                id: colony.id,
                x: colony.x * layer1.canvas.width,
                y: colony.y * layer1.canvas.height,
                radius: colony.major_length * 1.25 * down_scale,
                linecolor: color,
                defaultLinecolor: color,
                canvas: layer1.canvas,
                excluded_by: colony.excluded_by,
                area: colony.area,
                hash: hash
            }));
        }
        else {
            included_colonies.set(hash, new Circle({
                id: colony.id,
                x: colony.x * layer1.canvas.width,
                y: colony.y * layer1.canvas.height,
                radius: colony.major_length * 1.25 * down_scale,
                linecolor: selected_colony_color,
                defaultLinecolor: selected_colony_color,
                canvas: layer1.canvas,
                excluded_by: "",
                area: colony.area,
                hash: hash
            }));
        }
    });

    included_by_server = included_colonies.size;
    if (changed_settings.changed_algorithm) SetColoniesToPick();

    excluded_by_user.forEach((value, key) => {
        included_colonies.delete(key);
        excluded_colonies.set(key, value);
    });
    included_by_user.forEach((value, key) => {
        excluded_colonies.delete(key);
        included_colonies.set(key, value);
    });

    printPositions();

    clearTimeout(loading_timeout_id);
    document.body.classList.remove('wait');

    changed_settings.processed = true;
    if (changed_settings.new_request) {
        UpdateDetectionSettings(changed_settings.id);
        changed_settings.new_request = false;
    }
}
// Print colonies
var show_excluded = false;
var show_selected = true;
function printPositions() {
    layer1.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);

    UpdateNumberOfColonies();

    // Print new positions
    if (show_excluded) {
        for (let colony of excluded_colonies.values()) {
            colony.draw();
        }
    }
    if (show_selected) {
        for (let colony of included_colonies.values()) {
            colony.draw();
        }
    }
}

function PrintSelectedColonies(ids) {
    layer1.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
    for (let id of ids) {
        for (let colony of included_colonies.values()) {
            if (colony.get("id") == id) colony.draw();
        }
    }
}

function UpdateNumberOfColonies() {
    number_of_colonies = included_colonies.size;
    let number = $('#detected-colonies-number');
    if (number.text() != number_of_colonies) {
        number.hide();
        number.text(number_of_colonies);
        number.fadeIn();
    }
    $('#strategy-tab-button').prop("disabled", number_of_colonies <= 0);
    return number_of_colonies;
}

function SetColoniesToPick() {
    let excluded_user = [], included_user = [];
    let included = new Map(), excluded = new Map();
    colony_array.forEach((colony, index) => {
        let hash = colony.x + ',' + colony.y;
        if (colony.excluded_by == "" && excluded_by_user.has(hash)) {
            excluded_user.push(colony.id);
            excluded.set(hash, excluded_by_user.get(hash));
        }
        else if (included_by_user.has(hash) && colony.excluded_by != "" && !included.has(hash)) {
            included_user.push(colony.id);
            included.set(hash, included_by_user.get(hash));
        }
    });
    included_by_user = included;
    excluded_by_user = excluded;
    number_of_colonies = included_by_server - excluded_by_user.size + included_by_user.size;
    document.getElementById('enter-overview-button').onclick = function () {
        api('setcoloniestopick', {
            'job': current_job.id, 'ex_user': { "elements": excluded_user },
            'in_user': { "elements": included_user }, 'number': parseInt(document.getElementById('slider-max_number_of_coloniesstrategy-form').value)
        });
    }
    return number_of_colonies;
}

function drawTooltip(colony) {
    layer2.context.clearRect(0, 0, layer1.canvas.width, layer1.canvas.height);
    layer2.context.font = "12px sans-serif";
    colony.draw(layer2.context, 3);
    const colony_pos = colony.getPosition();
    let tooltip_pos = { x: colony_pos.x, y: colony_pos.y + colony.getRadius() * 1.5, width: 10, height: 15 };

    let text = "Colony #" + colony.get('id') + "\nArea: " + (Math.round(colony.get('area') * 1000) / 1000);
    if (colony.get('excluded_by') != "") {
        text += "\nExcluded by " + colony.get('excluded_by');
    }
    else if (colony.get('included_by')) {
        text += "\nIncluded by " + colony.get('included_by');
    }

    let lines = text.split("\n");
    lines.forEach(line => {
        tooltip_pos.width = Math.max(tooltip_pos.width, layer2.context.measureText(line).width + 20);
        tooltip_pos.height += 16;
    });
    if (tooltip_pos.x + tooltip_pos.width > layer2.canvas.width) tooltip_pos.x -= tooltip_pos.x + tooltip_pos.width - layer2.canvas.width;
    if (tooltip_pos.y + tooltip_pos.height > layer2.canvas.height) tooltip_pos.y = colony_pos.y - colony.getRadius() * 1.5 - tooltip_pos.height;
    layer2.context.fillStyle = "white";

    layer2.context.fillRect(tooltip_pos.x, tooltip_pos.y, tooltip_pos.width, tooltip_pos.height);

    layer2.context.fillStyle = "black";
    layer2.context.textBaseline = "top";

    let y_offset = 10;
    lines.forEach(line => {
        layer2.context.fillText(line, tooltip_pos.x + 10, tooltip_pos.y + y_offset);
        y_offset += 16; // line-height
    });
}

let settings_color = {};
function GetDetectionAlgorithms(detection_algorithms) {
    algorithms = detection_algorithms;

    const algorithm_selection = document.getElementById("select-algorithm");
    while (algorithm_selection.firstChild) algorithm_selection.removeChild(algorithm_selection.firstChild);

    for (let id in algorithms) {
        let algorithm = algorithms[id];
        // create color lookup table
        settings_color[id] = {};
        for (let setting of algorithm.settings) {
            settings_color[id][setting.id] = setting.color;
        }
        let algorithm_option = document.createElement('option');
        algorithm_option.value = id;
        algorithm_option.text = `${algorithm.name} ${(algorithm.description && algorithm.description != "") ? `(${algorithm.description})` : ``}`;
        algorithm_option.title = algorithm.description;
        algorithm_selection.appendChild(algorithm_option);
        CreateDetectionAlgorithmSettings(id);
    }
    changed_settings.id = algorithm_selection.value;
    algorithm_selection.onchange = function () {
        $('#' + this.value + '-settings').collapse('show');
        UpdateDetectionSettings(this.value)
    };
}

function CreateDetectionAlgorithmSettings(id) {
    const detection_settings = document.getElementById("detection-settings");

    let html = `<div id="${id}-settings" class="collapse" data-parent="#detection-settings">
    <form id="${id}-settings-form">
    `;
    let settings = new FormGroup(algorithms[id], id + "-settings-form");
    html += settings.getHtml() + `<button type="reset" class="btn btn-primary btn-sm w-100 mt-1">Reset to Default Values</button></form></div>`;
    detection_settings.insertAdjacentHTML("beforeend", html);
    settings.AddInputEvents();
    FormGroup.AddFormEvents(id + "-settings-form");
    $('#' + id + '-settings-form').change(function () {
        UpdateDetectionSettings(id);
    });
}
var loading_timeout_id;
var UpdateDetectionSettings = function (id) {
    // Send as:
    // {
    //  job_id: "222",
    //  algorithm: "321",
    //  settings:
    //  {
    //      "1234": 6,
    //      "123": true
    //  }
    // }
    if (changed_settings.processed) {
        document.body.classList.add('wait');
        let form = document.getElementById(id + '-settings-form');
        clearTimeout(loading_timeout_id);
        loading_timeout_id = setTimeout(function () {
            document.body.classList.remove('wait');
            ShowAlert("Colony Detection Timeout", "danger");
            changed_settings.new_request = false;
            changed_settings.processed = true;
        }, 4000);
        const form_data = new FormData(form);
        const algorithm_id = document.getElementById('select-algorithm').value;
        let new_settings = {
            job_id: current_job.id,
            algorithm: algorithm_id,
            settings: FormGroup.ReadForm(algorithms[algorithm_id], form_data)
        };
        console.log("Update Settings:", new_settings);
        api('updatedetectionsettings', new_settings);
        changed_settings.processed = false;
        changed_settings.changed_algorithm = changed_settings.id != id;
        changed_settings.id = id;
        drawSafetyMargin(new_settings.settings.safety_margin_lt, new_settings.settings.safety_margin_rb);
    }
    else {
        changed_settings.id = id;
        changed_settings.new_request = true;
    }
}