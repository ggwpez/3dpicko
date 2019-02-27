/* global $, multirange*/
/* exported FormGroup */
// TODO extend to Form?
class FormGroup{
    constructor(template, form_id = "", values = ""){
        this.template = template;
        this.form_id = form_id;
        this.values = values;
        this.html = ``;
        if(values === ""){
            for(let setting of this.template.settings){
                this.html += this.CreateFormGroupHtml(setting);
            }
        }
        else{
            for(let setting of this.template.settings){
                this.html += this.CreateFormGroupHtml(setting, values.settings[setting.id]);
            }
        }
    }

    /*
    Public Methods
    */

    // TODO create DOM and immediately apply events
    // applies necessary events to a forms input elements:
    AddInputEvents(){
        let form = document.getElementById(this.form_id);
        // enable multirange
        form.querySelectorAll('input[type=range][multiple]:not(.multirange)').forEach(multirange);
        // enable auto resize on number input
        $(form.querySelectorAll('input[type="number"]')).on('input', function(){
            if (this.value.length>0) this.style.width = this.value.length + 0.5 + "ch";
        }).trigger('input');
        // select value on focus
        // $(form).on('focus', 'input', function(){this.select();});
    }

    // applies necessary events to a form
    static AddFormEvents(form_id, onsubmit_function = null, unsaved_elements = null){
        let form = document.getElementById(form_id);
        // assign onsubmit action
        if(onsubmit_function) form.addEventListener('submit', onsubmit_function);
        // properly reset rangeslider
        form.addEventListener('reset', function(){
            let sliders = this.querySelectorAll('input[type=range].multirange.original');
            setTimeout(function() { sliders.forEach(input => {input.value = input.defaultValue;}); });
        });
        // set as unsaved on change, set as saved on submit/reset
        if(unsaved_elements){
            $(form).change(function(){
                if(!(this.getAttribute('id') in unsaved_elements)) unsaved_elements[this.getAttribute('id')] = this.dataset.description;
            });
            form.addEventListener('submit', function(){
                delete unsaved_elements[this.getAttribute('id')];
            });
            form.addEventListener('reset', function(){
                delete unsaved_elements[this.getAttribute('id')];
            });
        }
    }

    getHtml(){
        return this.html;
    }

    static ReadForm({settings}, form_data){
        console.log("Settings:",settings);
        let values = {};
        for (let setting of settings){
            let id = setting.id;
            if (setting.type == 'number' || setting.type == 'slider' || setting.type == 'range') values[id] = Number(form_data.get(id));
            else if (setting.type == 'checkbox') values[id] = form_data.has(id); // as boolean
            else if (setting.type == 'rangeslider') values[id] = {min: Number(form_data.getAll(id)[0]), max: Number(form_data.getAll(id)[1])};
            else if (setting.type == 'vector3') values[id] = {x: Number(form_data.getAll(id)[0]), y: Number(form_data.getAll(id)[1]), z: Number(form_data.getAll(id)[2])};
            else if (setting.type == 'vector2') values[id] = {x: Number(form_data.getAll(id)[0]), y: Number(form_data.getAll(id)[1])};
            else if (form_data.has(id)) values[id] = form_data.get(id); // strings: text, hidden, select, radio, unknown...
        }
        console.log(values);
        return values;
    }

    /*
    Private Methods
    */
    CreateNumberInputHtml(id, min, max, step = 1, value = "", placeholder = 0, required = true){
        value = Math.round(value * (1/step)) / (1/step);
        return `<input type="number" id="${id}" name="${id}" class="d-inline form-control-plaintext" ${required?`required="required"`:``} placeholder="${placeholder}" ${(min!="")?`min=${min}`:``} ${(max!="")?`max=${max}`:``} ${(step)?`step=${step}`:`1`} value="${value}">`;
    }

    CreateRadioOptionHtml(id, option_id, option, value, form_id){
        return `<div class="custom-control custom-radio">
        <input class="custom-control-input" type="radio" name="${id}" id="${option_id}${form_id}" value="${option_id}" ${(value==option_id)?`checked`:``}>
        <label class="custom-control-label font-weight-normal" for="${option_id}${form_id}">${option}</label>
        </div>`;
    }

    CreateSelectOptionHtml(option_id, option, value){
        return `<option value=${option_id} ${(value==option_id)?`selected`:``}>${option}</option>`;
    }

    CreateFormGroupHtml({id, name, type, defaultValue = "", value = "", description="", min="" , max="", step=1, unit="", options={}, placeholder = 0, required = true}, new_value = ""){
        // console.log("Input-Field:",{id, name, type, defaultValue, value, description, min , max, step, unit, options, placeholder, required, new_value});
        if(new_value !== "") value = new_value;
        else if(value === "") value = defaultValue;
        let html = `<div class="form-group ${(type=="slider")?`text-wrap`:``}"><label for="${id}">${name}:</label>`;

        if (type == "number"){
            html += `${this.CreateNumberInputHtml(id, min, max, step, value)} ${unit}.`;
        }
        else if (type == "vector3"){
            html += `(${this.CreateNumberInputHtml(id, min, max, step, value.x)},${this.CreateNumberInputHtml(id, min, max, step, value.y)},${this.CreateNumberInputHtml(id, min, max, step, value.z)}) ${unit}.`;
        }
        else if (type == "vector2"){
            html += `(${this.CreateNumberInputHtml(id, min, max, step, value.x)},${this.CreateNumberInputHtml(id, min, max, step, value.y)}) ${unit}.`;
        }
        else if (type == "slider"){
            value = Math.round(value * (1/step)) / (1/step);
            html += `<br>
            <input type="range" class="custom-range" id="slider-${id}${this.form_id}" name="${id}" min="${min}" max="${max}" step="${step}" value="${value}" oninput="$('#number-${id}${this.form_id}')[0].value=this.value;$('#number-${id}${this.form_id}').trigger('input');">
            <div style="text-align: center;"><span style="float:left;">${min}</span>
            <input style="max-width: 40%; text-align: center;" type="number" min="${min}" max="${max}" step="${step}" id="number-${id}${this.form_id}" value="${value}" oninput="$('#slider-${id}${this.form_id}')[0].value=this.value;">
            <span style="float:right">${max}</span></div>
            `;
        }
        else if (type == "rangeslider"){
            value.min = Math.round(value.min * (1/step)) / (1/step);
            value.max = Math.round(value.max * (1/step)) / (1/step);
            html += `<br>
            <input type="range" class="custom-range" id="slider-${id}${this.form_id}" multiple value="${value.min},${value.max}" min="${min}" max="${max}" step="${step}" oninput="$('#number-${id}${this.form_id}-min')[0].value=this.valueLow;$('#number-${id}${this.form_id}-min').trigger('input');$('#number-${id}${this.form_id}-max')[0].value=this.valueHigh;$('#number-${id}${this.form_id}-max').trigger('input');" >
            <div style="text-align: center;"><span style="float:left;">${min}</span>
            <input style="max-width: 20%; text-align: center;" type="number" min="${min}" max="${max}" step="${step}" id="number-${id}${this.form_id}-min" name="${id}" value="${value.min}" onchange="$('#slider-${id}${this.form_id}')[0].valueLow=this.value;$('#slider-${id}${this.form_id}').trigger('input');">
            <input style="max-width: 20%; text-align: center;" type="number" min="${min}" max="${max}" step="${step}" id="number-${id}${this.form_id}-max" name="${id}" value="${value.max}" onchange="$('#slider-${id}${this.form_id}')[0].valueHigh=this.value;$('#slider-${id}${this.form_id}').trigger('input');">
            <span style="float:right">${max}</span></div>
            `;
        }
        else if (type == "checkbox"){
            html += `<div class="custom-control custom-checkbox d-inline"><input type="checkbox" class="custom-control-input" id="${id}${this.form_id}" name="${id}" ${(value)?`checked`:``}><label class="custom-control-label" for="${id}${this.form_id}"></label></div>`;
        }
        else if (type == "radio"){
            for (let option_id in options){
                html += this.CreateRadioOptionHtml(id, option_id, options[option_id], value, this.form_id);
            }
        }
        else if (type == "dropdown"){
            html += `<select class="form-control" id="${id}" name="${id}">`;
            for (let option_id in options){
                html += this.CreateSelectOptionHtml(option_id, options[option_id], value);
            }
            html += `</select>`;
        }
        else if (type == "text"){
            html += `<input type="text" id="${id}" class="form-control" name="${id}" placeholder="${placeholder}" ${(min!="")?`minlength=${min}`:``} ${(max!="")?`maxlength=${max}`:``} value="${value}" ${required?`required="required"`:``}>`;
        }

        if (description && description.length>0) html += `<small class="form-text text-muted">${description}</small>`;

        return html += `</div>`;
    }
}