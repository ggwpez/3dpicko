/*
 * SPDX-FileCopyrightText: 2018-2020 Harald Gültig <hgueltig@gmail.com>
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

var upload_timeout_id;
var dropZone = document.getElementById('dropZone');
$(function () {
    DisableDropzone();
    window.addEventListener("dragover", function (e) {
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
        e.dataTransfer.dropEffect = 'none';
    }, false);
    window.addEventListener("drop", function (e) {
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
    }, false);
    document.getElementById('overlay').addEventListener("dragover", function (e) {
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
        e.dataTransfer.dropEffect = 'none';
    }, false);
    document.getElementById('overlay').addEventListener("drop", function (e) {
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
    }, false);
    dropZone.ondragover = function (event) {
        event.stopPropagation();
        event.preventDefault();
        if (dropZone.dataset.enabled) event.dataTransfer.dropEffect = 'copy';
        else event.dataTransfer.dropEffect = 'none';
    };
    dropZone.ondragenter = function (event) {
        if (dropZone.dataset.enabled) {
            event.dataTransfer.dropEffect = 'copy';
            dropZone.classList.add('dragover');
        }
        else event.dataTransfer.dropEffect = 'none';
    };
    dropZone.ondragleave = function () {
        dropZone.classList.remove('dragover');
    };
    dropZone.ondrop = function (event) {
        event.stopPropagation();
        event.preventDefault();
        if (dropZone.dataset.enabled) upload(event.dataTransfer.files[0]);
    };
    document.getElementById('classicUpload').addEventListener('change', function Change(e) {
        upload(this.files[0]);
    });
});

function DisableDropzone() {
    document.getElementById('continue-1').disabled = true;
    dropZone.dataset.enabled = false;
    if (dropZone.classList.contains('empty')) dropZone.innerHTML = "<div class='spinner-border'></div>";
    dropZone.onclick = null;
    dropZone.style.pointerEvents = 'none';
}

function EnableDropzone() {
    document.getElementById('continue-1').disabled = false;
    if (dropZone.classList.contains('empty')) dropZone.innerHTML = "Drop Image";
    dropZone.dataset.enabled = true;
    dropZone.style.pointerEvents = 'auto';
    dropZone.onclick = function () {
        $('#classicUpload').trigger('click');
    };
}

function upload(file) {
    dropZone.classList.remove('dragover');
    if (file) {
        dropZone.classList.add('empty');
        DisableDropzone();
        upload_timeout_id = setTimeout(function () {
            EnableDropzone();
            ShowAlert("Image Upload Timeout", "danger");
        }, 10000);
        let rawData = new ArrayBuffer();    // TODO
        let reader = new FileReader();
        reader.readAsDataURL(file);
        reader.onload = function (e) {
            rawData = e.target.result;
            var base64result = rawData.substr(rawData.indexOf(',') + 1);
            api("uploadimage", { original_filename: file.name, file: base64result });
        };
    }
}