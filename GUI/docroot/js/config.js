var default_profiles = {
  "printer-profile": "ABC",
  "socket-profile": "",
  "plate-profile": "LARGEPLATE"
}

var UpdateSettingsProfile = function (e){
  // var send = {
  //   id: "XYZ",
  //   type: "printer-profile",
  //   profile_name: "Example Printer",
  //   settings: {
  //     "cut_filament_position_above_trigger": {x: 1,y: 2, z: 3},
  //     "z_coordinate_pushing_the_trigger": 4,
  //     ...
  //   }
  // };
  e.preventDefault();
  
  const form_data = new FormData(this);
  let type = form_data.get('type');
  let value = "";
  var json_object = {
    id: form_data.get('id'),
    type: form_data.get('type'),
    profile_name: form_data.get('profile_name'),
  };
  json_object.settings = ReadSettings(profile_templates[type].settings, form_data);
  // console.log({json_object});
  
  $('#collapse-'+json_object.id).collapse('hide');
  if (json_object.id == "new-printer-profile" || json_object.id == "new-socket-profile" || json_object.id == "new-plate-profile"){
    $('#form-new-'+type)[0].reset();
    api("createsettingsprofile", json_object);
  } 
  else api("updatesettingsprofile", json_object);
}

function ReadSettings(template, form_data){
  let settings = {};
  for (let id in template){
    if (template[id].type == 'number' || template[id].type == 'slider' || template[id].type == 'range') settings[id] = Number(form_data.get(id));
    else if (template[id].type == 'checkbox') settings[id] = form_data.has(id);
    else if (template[id].type == 'rangeslider') settings[id] = {min: Number(form_data.getAll(id)[0]), max: Number(form_data.getAll(id)[1])};
    else if (template[id].type == 'vector3') settings[id] = {x: Number(form_data.getAll(id)[0]), y: Number(form_data.getAll(id)[1]), z: Number(form_data.getAll(id)[2])};
    else if (template[id].type == 'vector2') settings[id] = {x: Number(form_data.getAll(id)[0]), y: Number(form_data.getAll(id)[1])};
    else if (form_data.has(id)) settings[id] = form_data.get(id);
  }
  return settings;
}

$(function LoadProfiles(){
  // empty "new-profiles"
  AddProfileToList(profile_templates['printer-profile']);
  AddProfileToList(profile_templates['socket-profile']);
  AddProfileToList(profile_templates['plate-profile']);
  // debugging profiles
  AddProfileToList(example_printer);
  AddProfileToList(makergear);
  AddProfileToList(creality);  
  AddProfileToList(prototyp1);
  AddProfileToList(default_plate);
  AddProfileToList(large_plate);
});

function CreateFormGroupHtml({id, name, type, value, description="", min="" , max="", step=1, unit="", options={}}, form_id = "", new_value = ""){
  if(new_value !== "") value = new_value;
  // console.log("Input-Field:",{id, name, type, value, description, min , max, step, unit, options});
  let form_group = `<div class="form-group ${(type=="slider")?`text-wrap`:``}"><label for="${id}">${name}:</label>`;
  
  if (type == "number"){
    form_group += `${CreateNumberInputHtml(id, min, max, step, value)} ${unit}.`;
  }
  else if (type == "vector3"){
    form_group += `(${CreateNumberInputHtml(id, min, max, step, value.x)},${CreateNumberInputHtml(id, min, max, step, value.y)},${CreateNumberInputHtml(id, min, max, step, value.z)}) ${unit}.`;
  }
  else if (type == "vector2"){
    form_group += `(${CreateNumberInputHtml(id, min, max, step, value.x)},${CreateNumberInputHtml(id, min, max, step, value.y)}) ${unit}.`;
  }
  else if (type == "slider"){
    form_group += `<br>
    <input type="range" class="custom-range" id="slider-${id}${form_id}" name="${id}" min="${min}" max="${max}" step="${step}" value="${value}" oninput="$('#number-${id}${form_id}')[0].value=this.value;$('#number-${id}${form_id}').trigger('input');">
    <div style="text-align: center;"><span style="float:left;">${min}</span>
    <input style="max-width: 40%; text-align: center;" type="number" min="${min}" max="${max}" step="${step}" id="number-${id}${form_id}" value="${value}" oninput="$('#slider-${id}${form_id}')[0].value=this.value;">
    <span style="float:right">${max}</span></div>
    `;
  }
  else if (type == "rangeslider"){
    form_group += `<br>
    <input type="range" class="custom-range" id="slider-${id}${form_id}" multiple value="${value.min},${value.max}" min="${min}" max="${max}" step="${step}" oninput="$('#number-${id}${form_id}-min')[0].value=this.valueLow;$('#number-${id}${form_id}-min').trigger('input');$('#number-${id}${form_id}-max')[0].value=this.valueHigh;$('#number-${id}${form_id}-max').trigger('input');" >
    <div style="text-align: center;"><span style="float:left;">${min}</span>
    <input style="max-width: 20%; text-align: center;" type="number" min="${min}" max="${max}" step="${step}" id="number-${id}${form_id}-min" name="${id}" value="${value.min}" onchange="$('#slider-${id}${form_id}')[0].valueLow=this.value;$('#slider-${id}${form_id}').trigger('input');">
    <input style="max-width: 20%; text-align: center;" type="number" min="${min}" max="${max}" step="${step}" id="number-${id}${form_id}-max" name="${id}" value="${value.max}" onchange="$('#slider-${id}${form_id}')[0].valueHigh=this.value;$('#slider-${id}${form_id}').trigger('input');">
    <span style="float:right">${max}</span></div>
    `;
  }
  else if (type == "checkbox"){
    form_group += `<div class="custom-control custom-checkbox d-inline"><input type="checkbox" class="custom-control-input" id="${id}${form_id}" name="${id}" ${(value)?`checked`:``}><label class="custom-control-label" for="${id}${form_id}"></label></div>`;
  }
  else if (type == "radio"){
    for (let option_id in options){
      form_group += CreateRadioOptionHtml(id, option_id, options[option_id], value, form_id);
    }
  }
  else if (type == "dropdown"){
    form_group += `<select class="form-control" id="${id}" name="${id}">`;
    for (let option_id in options){
      form_group += CreateSelectOptionHtml(option_id, options[option_id], value);
    }
    form_group += `</select>`;
  }
  
  if (description && description.length>0) form_group += `<small class="form-text text-muted">${description}</small>`;
  form_group += `</div>`;
  return form_group;
}

function CreateNumberInputHtml(id, min, max, step = 1, value = "", placeholder = 0, required = true){
  return `<input type="number" id="${id}" name="${id}" class="d-inline form-control-plaintext" ${required?`required="required"`:``} placeholder="${placeholder}" ${(min!="")?`min=${min}`:``} ${(max!="")?`max=${max}`:``} ${(step)?`step=${step}`:`1`} value="${value}">`;
}

function CreateRadioOptionHtml(id, option_id, option, value, form_id){
  return `<div class="custom-control custom-radio">
  <input class="custom-control-input" type="radio" name="${id}" id="${option_id}${form_id}" value="${option_id}" ${(value==option_id)?`checked`:``}>
  <label class="custom-control-label font-weight-normal" for="${option_id}${form_id}">${option}</label>
  </div>`;
}

function CreateSelectOptionHtml(option_id, option, value){
  return `<option value=${option_id} ${(value==option_id)?`selected`:``}>${option}</option>`;
}

function AddProfileToList(profile){
  if(profile.id in all_profiles){
    // update profile (delete old profile)
    // TODO check if new values
    DeleteProfile(profile.id);
  }
  
  let new_profile = false;
  if(profile.id == "new-"+profile.type){
    new_profile = true;
  }
  else all_profiles[profile.id] = profile;
  
  let html = `
  <div class="card" id="card-${profile.id}">
  <div class="card-header" data-toggle="collapse" data-target="#collapse-${profile.id}">
  <span id="default-label-{profile.id}" class="badge badge-primary" ${(default_profiles[profile.type]==profile.id)?``:`style="display: none;"`}>Default</span>
  <h4 class="btn btn-link">
  ${(new_profile)?``:`<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="printer-profile" data-id="${profile.id}">&times;</button>`}
  ${profile.profile_name}
  </h4>
  </div>
  <div id="collapse-${profile.id}" class="collapse" data-parent="#group">
  <div class="card-body">
  <form id="form-${profile.id}" name="form-${profile.id}" method="post" enctype="multipart/form-data">
  <input type="hidden" id="id" name="id" value="${profile.id}">
  <input type="hidden" id="type" name="type" value="${profile.type}">
  <div class="form-group">
  <label for="profile_name">Profile identifier:</label>
  <input required="required" id="profile_name" class="form-control" name="profile_name" type="text" placeholder="choose identifier" value="${(new_profile)?``:profile.profile_name}">
  </div>`;

  let template = profile_templates[profile.type].settings;
  for(let setting_id in template){
    let setting = template[setting_id];
    setting.id = setting_id;
    if(typeof(profile.settings[setting_id]) === 'object'&&profile.settings[setting_id].name) html += CreateFormGroupHtml(profile.settings[setting_id], profile.id);
    else html += CreateFormGroupHtml(setting, profile.id, profile.settings[setting_id]);
  }
  if(new_profile) html += `<button type="submit" class="btn btn-primary">Create profile</button>`;
  else html += `<button type="submit" class="btn btn-primary mr-2">Save changes</button><button type="button" class="btn btn-outline-primary" onclick="SetDefaultProfile('${profile.id}');">Set as Default</button>`;
  html += `</form></div></div></div>`;
  document.getElementById(profile.type+'s').insertAdjacentHTML('beforeend',html);

  multirange.init();
  $('#form-'+profile.id).on('submit', UpdateSettingsProfile);
  $('#form-'+profile.id).on('focus', 'input', function(){this.select();});
  $('#form-'+profile.id+' input[type="number"]').on('input', function(){
    if (this.value.length>0) this.style.width = this.value.length + 0.5 + "ch";
  }).trigger('input');

  if(!(new_profile) && (profile.type == "printer-profile" || profile.type == "socket-profile")){
    if(profile.id == default_profiles[profile.type]){
      document.getElementById("select-"+profile.type).add(new Option(profile.profile_name,  profile.id, true, true), 0);
    }
    else document.getElementById("select-"+profile.type).add(new Option(profile.profile_name, profile.id));
  }
}

function SetDefaultProfile(id){
  if(id in all_profiles){
    let profile = all_profiles[id];
    default_profiles[profile.type] = id;
    console.log(default_profiles);
    $(`#${profile.type}s .card-header .badge`).hide();
    $(`#${profile.type}s #card-${id} .badge`).show();
    document.getElementById("select-"+profile.type).value = id; 
    api("setdefaultsettingsprofile", id);
    ShowAlert("Set Profile "+profile.profile_name+" as Default");
  }
}

function DeleteProfile(id){
  if(id in all_profiles){
    $('#card-'+id).remove();
    const profile = all_profiles[id];
    switch(profile.type){
      case "printer-profile": 
      $('#select-printer-profile option[value='+id+']').remove();
      break;
      case "socket-profile": 
      $('#select-socket-profile option[value='+id+']').remove();
      break;
      case "plate-profile": 
      // TODO update options list
      break;
    }
    delete all_profiles[id];
  }
}

let profile_templates = {
  "printer-profile": {
    id : "new-printer-profile",
    type : "printer-profile",
    profile_name : "Create New Printer Profile &gt;",
    settings: {
      "cut_filament_position_above_trigger": {
        name: "Cut position",
        type: "vector3",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The (x,y,z) position, in the coordinate system of the printer, the nozzle needs to move to in order to be above the trigger of the scissor and directly above the center of the space between the scissors blades."
      },
      "z_coordinate_pushing_the_trigger": {
        name: "Push trigger at",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The global z coordinate (at x and y of cut-position) the trigger of the scissor is definitely pushed."
      },    
      "distance_between_pushed_trigger_and_gap_between_scissors_blade": {
        name: "Cut distance",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The distance between the nozzle when the trigger is pushed and the gap between the scissors blades, where the filament will be cut."
      },
      "movement_speed": {
        name: "Movement speed",
        type: "number",
        min: 0,
        step: 1,
        value: "",
        unit: "mm/min",
        description: "The speed the nozzle is moved with."
      },
      "filament_extrusion_length_on_move_offset": {
        name: "Filament offset (move)",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "Offset to the length up to which the filament will be extruded while moving the nozzle above the plates."
      },
      "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset": {
        name: "Filament offset (pick)",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "Offset to the length up to which the filament will be extruded when picking from source- and placing on masterplate."
      }
    }
  },
  "socket-profile" : {
    id : "new-socket-profile",
    type : "socket-profile",
    profile_name : "Create New Socket Profile &gt;",
    settings: {
      "socket_origin_offset": {
        name: "Socket origin offset",
        type: "vector3",
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The (x,y,z) offset of the socket origin (with z-origin on socket surface) to the origin of the printers coordinate system (home)."
      },
      "global_origin_of_source_plate": {
        name: "Source plate cutout origin",
        type: "vector2",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The (x,y) origin of the slot/cut-out of the source plate given as a point of the coordinate system of the socket."
      },    
      "depth_of_cutout_the_source_plate_lies_in": {
        name: "Source plate cutout depth",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The source plate cutout depth."
      },
      "global_origin_of_master_plate": {
        name: "Master plate cutout origin",
        type: "vector2",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The (x,y) origin of the slot/cut-out of the master plate given as a point of the coordinate system of the socket."
      },    
      "depth_of_cutout_the_master_plate_lies_in": {
        name: "Master plate cutout depth",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The master plate cutout depth."
      },
      "global_origin_of_goal_plate": {
        name: "Goal plate cutout origin",
        type: "vector2",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The (x,y) origin of the slot/cut-out of the goal plate given as a point of the coordinate system of the socket."
      },    
      "depth_of_cutout_the_goal_plate_lies_in": {
        name: "Goal plate cutout depth",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The goal plate cutout depth."
      },
      "orientation_of_goal_plate" : {
        name: "Goal and master plate orientation",
        type: "radio",
        options: {
          kFirstRowFirstColumnAtCutoutOrigin : "Well 'A1' at cutout origin.",
          kLastRowFirstColumnAtCutoutOrigin : "Well 'm1' at cutout origin. (m=last row)"
        },
        value: "kFirstRowFirstColumnAtCutoutOrigin", 
        description: "Orientation of goal- and masterplate compared to the cutout it is lying in."
      }
    }
  },
  "plate-profile" : {
    id : "new-plate-profile",
    type : "plate-profile",
    profile_name : "Create New Plate Profile &gt;",
    settings: {
      "number_of_rows": {
        name: "Number of rows",
        type: "number",
        step: 1,
        value: "",
        unit: "",
        description: "Number of rows the goal plate has."
      },
      "number_of_columns": {
        name: "Number of columns",
        type: "number",
        step: 1,
        value: "",
        unit: "",
        description: "Number of columns the goal plate has."
      },
      "a1_row_offset": {
        name: "A1 row offset:",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The offset of the center of the first well A1 to the upper edge of the goal plate."
      },    
      "a1_column_offset": {
        name: "A1 column offset",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The offset of the center of the first well A1 to the left edge of the goal plate."
      }, 
      "well_spacing_center_to_center": {
        name: "Well spacing center to center",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The distance between the center of a well to the center of any directly adjacent well."
      }, 
      "height_source_plate": {
        name: "Source plate height",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The height of the source plate."
      },
      "height_master_plate": {
        name: "Master plate height",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The height of the master plate."
      }, 
      "height_goal_plate": {
        name: "Goal plate height",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The height of the goal plate."
      },  
      "well_depth": {
        name: "Well depth",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The depth of every well."
      }, 
      "culture_medium_thickness": {
        name: "Culture medium thickness",
        type: "number",
        min: 0,
        step: 0.01,
        value: "",
        unit: "mm",
        description: "The thickness of the used culture medium inside the source and master plate, for instance agars."
      }
    }
  }
};

/*
Local Profiles for debugging
*/

let example_printer = {
  id : "PrinterXYZ",
  type : "printer-profile",
  profile_name : "Example Printer",
  settings: {
    "cut_filament_position_above_trigger": {
      name: "Cut position",
      type: "vector3",
      min: 0,
      step: 0.01,
      value: {x:11,y:12,z:13},
      unit: "mm",
      description: "The (x,y,z) position, in the coordinate system of the printer, the nozzle needs to move to in order to be above the trigger of the scissor and directly above the center of the space between the scissors blades."
    },
    "z_coordinate_pushing_the_trigger": {
      name: "Push trigger at",
      type: "number",
      min: 0,
      step: 0.01,
      value: 14,
      unit: "mm",
      description: "The global z coordinate (at x and y of cut-position) the trigger of the scissor is definitely pushed."
    },    
    "distance_between_pushed_trigger_and_gap_between_scissors_blade": {
      name: "Cut distance",
      type: "number",
      min: 0,
      step: 0.01,
      value: 15,
      unit: "mm",
      description: "The distance between the nozzle when the trigger is pushed and the gap between the scissors blades, where the filament will be cut."
    },
    "movement_speed": {
      name: "Movement speed",
      type: "number",
      min: 0,
      step: 1,
      value: 16,
      unit: "mm/min",
      description: "The speed the nozzle is moved with."
    },
    "filament_extrusion_length_on_move_offset": {
      name: "Filament offset (move)",
      type: "number",
      min: 0,
      step: 0.01,
      value: 17,
      unit: "mm",
      description: "Offset to the length up to which the filament will be extruded while moving the nozzle above the plates."
    },
    "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset": {
      name: "Filament offset (pick)",
      type: "number",
      min: 0,
      step: 0.01,
      value: 18,
      unit: "mm",
      description: "Offset to the length up to which the filament will be extruded when picking from source- and placing on masterplate."
    },
    "ttrtwtr":
    {
      name: "Zahl",
      type: "slider",
      valueType: "float",
      min: -10,
      max: 10,
      step: 1,
      value: 1,
      description: ""
    }
  }
};

let prototyp1 = {
  "id" : "DEFAULTBLUE",
  "type" : "socket-profile",
  "profile_name" : "Default Socket Blue",
  settings: {
    "socket_origin_offset" : {x: 1, y: 2, z: 3},
    "global_origin_of_source_plate" : {x:4, y:5},
    "depth_of_cutout_the_source_plate_lies_in" : 6,
    "global_origin_of_master_plate" : {x:7,y:8},
    "depth_of_cutout_the_master_plate_lies_in" : 9,
    "global_origin_of_goal_plate" : {x:10,y:11},
    "depth_of_cutout_the_goal_plate_lies_in" : 12,
    "orientation_of_goal_plate" : "kLastRowFirstColumnAtCutoutOrigin"
  }
};
let default_plate = {
  "id" : "DEFALUTPLATE",
  "type" : "plate-profile",
  "profile_name" : "Default Plates - 96 Wells",
  settings:{
    "number_of_rows" : 8,
    "number_of_columns" : 12,
    "a1_row_offset" : 1,
    "a1_column_offset" : 2,
    "well_spacing_center_to_center" : 3,
    "height_source_plate" : 4,
    "height_master_plate" : 5,
    "height_goal_plate" : 6,
    "well_depth" : 7,
    "culture_medium_thickness" : 8
  }
};
let large_plate = {
  "id" : "LARGEPLATE",
  "type" : "plate-profile",
  "profile_name" : "LargePlate - 16x24=384 Wells",
  settings:{
    "number_of_rows" : 16,
    "number_of_columns" : 24,
    "a1_row_offset" : 1,
    "a1_column_offset" : 2,
    "well_spacing_center_to_center" : 3,
    "height_source_plate" : 4,
    "height_master_plate" : 5,
    "height_goal_plate" : 6,
    "well_depth" : 7,
    "culture_medium_thickness" : 8
  }
};
let makergear = {
  "id" : "XYZ",
  "type" : "printer-profile",
  "profile_name" : "Makergear M3",
  settings:{
    "cut_filament_position_above_trigger" : {x:1,y:2,z:3},
    "z_coordinate_pushing_the_trigger" : 4,
    "distance_between_pushed_trigger_and_gap_between_scissors_blade" : 5,
    "movement_speed" : 6,
    "filament_extrusion_length_on_move_offset": 7,
    "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : 8
  }
};
let creality = {
  "id" : "ABC",
  "type" : "printer-profile",
  "profile_name" : "Creality Ender-3",
  settings:{
    "cut_filament_position_above_trigger" : {x:1,y:2,z:3},
    "z_coordinate_pushing_the_trigger" : 4,
    "distance_between_pushed_trigger_and_gap_between_scissors_blade" : 5,
    "movement_speed" : 6,
    "filament_extrusion_length_on_move_offset": 7,
    "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : 8
  }
};