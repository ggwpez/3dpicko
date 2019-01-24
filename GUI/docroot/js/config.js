var printer_profiles = {}, socket_profiles = {}, plate_profiles = {};

var UpdateSettingsProfile = function (e){
  e.preventDefault();
  
  const form_data = new FormData(this);
  var json_object = {};
  for (const [key, value]  of form_data.entries()) {
    json_object[key] = value;
  }
  if(json_object.id == "newprinter-profile" || json_object.id == "newsocket-profile" || json_object.id == "newplate-profile") api("createsettingsprofile", json_object);
  else api("updatesettingsprofile", json_object);
}

function DeletePrinterProfile(id){
  $('#card-'+id).remove();
  $('#select-printer-profile option[value='+id+']').remove();
  delete printer_profiles[id];
  // TODO Backend delete
}
function DeleteSocketProfile(id){
  $('#card-'+id).remove();
  $('#select-socket-profile option[value='+id+']').remove();
  delete socket_profiles[id];
  // TODO Backend delete
}
function DeletePlateProfile(id){
  $('#card-'+id).remove();
  delete plate_profiles[id];
  // TODO Backend delete
}

$('#delete-dialog').on('show.bs.modal', function (e) {
  // <button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="printer-profile" data-id="${printer_profile.id}">&times;</button>
  const type = $(e.relatedTarget).data('type');
  const id = $(e.relatedTarget).data('id');
  
  switch (type) {
    case "image":
    // TODO update createpickjob.js
    // document.getElementById("delete-dialog-body").innerHTML = "Bild löschen?";
    // document.getElementById("delete-button").onclick = function(){DeleteImage(id)}
    break;
    case "printer-profile":
    document.getElementById('delete-dialog-body').innerHTML = `Delete printer profile ${printer_profiles[id].profile_name}?`;
    document.getElementById('delete-button').onclick = function(){DeletePrinterProfile(id)};
    break;
    case "socket-profile":
    document.getElementById('delete-dialog-body').innerHTML = `Delete socket profile ${socket_profiles[id].profile_name}?`;
    document.getElementById('delete-button').onclick = function(){DeleteSocketProfile(id)};
    break;
    case "plate-profile":
    document.getElementById('delete-dialog-body').innerHTML = `Delete plate profile ${plate_profiles[id].profile_name}?`;
    document.getElementById('delete-button').onclick = function(){DeletePlateProfile(id)};
    break;
  }
});
$(function LoadProfiles(){
  newprinterprofile = {
    "id" : "newprinter-profile",
    "type" : "printer-profile",
    "profile_name" : "",
    "cut_filament_position_x" : "",
    "cut_filament_position_y" : "",
    "filament_extrusion_length" : "",
    "z_coordinate_extruded_filament_above_plates" : "",
    "z_coordinate_extruded_filament_inside_source_plate" : "",
    "z_coordinate_extruded_filament_inside_master_plate" : "",
    "z_coordinate_extruded_filament_inside_goal_plate" : ""
  };
  AddPrinterProfile(newprinterprofile);

  newsocketprofile = {
    "id" : "newsocket-profile",
    "type" : "socket-profile",
    "profile_name" : "",
    "global_origin_of_source_plate_x" : "",
    "global_origin_of_source_plate_y" : "",
    "global_origin_of_master_plate_x" : "",
    "global_origin_of_master_plate_y" : "",
    "global_origin_of_goal_plate_x" : "",
    "global_origin_of_goal_plate_y" : "",
    "orientation_of_goal_plate" : "",
    "socket_origin_offset_x" : "",
    "socket_origin_offset_y" : ""
  };
  AddSocketProfile(newsocketprofile);

  prototyp1 = {
    "id" : "JKL",
    "type" : "socket-profile",
    "profile_name" : "Prototyp",
    "global_origin_of_source_plate_x" : 1,
    "global_origin_of_source_plate_y" : 2,
    "global_origin_of_master_plate_x" : 3,
    "global_origin_of_master_plate_y" : 4,
    "global_origin_of_goal_plate_x" : 5,
    "global_origin_of_goal_plate_y" : 6,
    "orientation_of_goal_plate" : 7,
    "socket_origin_offset_x" : 8,
    "socket_origin_offset_y" : 9
  };
  AddSocketProfile(prototyp1);

  newplateprofile = {
    "id" : "newplate-profile",
    "type" : "plate-profile",
    "profile_name" : "",
    "number_of_rows" : "",
    "number_of_columns" : "",
    "a1_row_offset" : "",
    "a1_column_offset" : "",
    "well_spacing_center_to_center" : ""
  };
  AddPlateProfile(newplateprofile);
  
  // TDOD Backend load
  makergear = {
    "id" : "XYZ",
    "type" : "printer-profile",
    "profile_name" : "Makergear M3",
    "cut_filament_position_x" : 1,
    "cut_filament_position_y" : 2,
    "filament_extrusion_length" : 3,
    "z_coordinate_extruded_filament_above_plates" : 4,
    "z_coordinate_extruded_filament_inside_source_plate" : 5,
    "z_coordinate_extruded_filament_inside_master_plate" : 6,
    "z_coordinate_extruded_filament_inside_goal_plate" : 7
  };

  creality = {
    "id" : "ABC",
    "type" : "printer-profile",
    "profile_name" : "Creality Ender-3",
    "cut_filament_position_x" : 1,
    "cut_filament_position_y" : 2,
    "filament_extrusion_length" : 3,
    "z_coordinate_extruded_filament_above_plates" : 4,
    "z_coordinate_extruded_filament_inside_source_plate" : 5,
    "z_coordinate_extruded_filament_inside_master_plate" : 6,
    "z_coordinate_extruded_filament_inside_goal_plate" : 7
  };
  AddPrinterProfile(makergear);
  AddPrinterProfile(creality);
});

function AddPrinterProfile(printer_profile){
  printer_profiles[printer_profile.id] = printer_profile;

  let link_text = printer_profile.profile_name;
  let button_text = "Save changes";
  let deletable = true;

  if(printer_profile.id == "newprinter-profile"){
    link_text = "Create new printer profile ->"; 
    button_text = "Create profile";
    deletable = false;
  } 

  const html = `
  <div class="card" id="card-${printer_profile.id}">
  <div class="card-header">
  <h2 class="mb-0">
  ${deletable ? `<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="printer-profile" data-id="${printer_profile.id}">&times;</button>` : ``}
  <button id="link-${printer_profile.id}" class="btn btn-link collapsed" type="button" data-toggle="collapse" data-target="#collapse-${printer_profile.id}">
  ${link_text}
  </button>
  </h2>
  </div>
  <div id="collapse-${printer_profile.id}" class="collapse" data-parent="#group">
  <div class="card-body">
  <form id="form-${printer_profile.id}" name="form-${printer_profile.id}" method="post" enctype="multipart/form-data">
  <input type="hidden" id="id" name="id" value="${printer_profile.id}">
  <input type="hidden" id="type" name="type" value="printer-profile">
  <div class="form-group">
  <label for="profile_name">Name of printer profile:</label>
  <input id="profile_name" class="form-control" name="profile_name" type="text" placeholder="name" value="${printer_profile.profile_name}" required="required">
  </div>
  <div class="form-group">
  <label>Cut-Position:</label>
  (<input id="cut_filament_position_x" name="cut_filament_position_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" min="0" step="0.1" value="${printer_profile.cut_filament_position_x}">,
  <input id="cut_filament_position_y" name="cut_filament_position_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${printer_profile.cut_filament_position_y}">)
  <small class="form-text text-muted">
  The xy position, in the coordinate system of the printer, the nozzle needs to move to in order to have the filament cut
  </small>
  </div>
  <div class="form-group">
  <label>Filament length:</label>
  <input id="filament_extrusion_length" name="filament_extrusion_length" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${printer_profile.filament_extrusion_length}">
  <small class="form-text text-muted">
  The length up to which the filament shall be extruded, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>z-coordinate:</label>
  <input id="z_coordinate_extruded_filament_above_plates" name="z_coordinate_extruded_filament_above_plates" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${printer_profile.z_coordinate_extruded_filament_above_plates}">
  <small class="form-text text-muted">
  The z coordinate, in the coordinate system of the printer, the extruded filament can be moved above the plates without touching them or anything else, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>z-coordinate Source:</label>
  <input id="z_coordinate_extruded_filament_inside_source_plate" name="z_coordinate_extruded_filament_inside_source_plate" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${printer_profile.z_coordinate_extruded_filament_inside_source_plate}">
  <small class="form-text text-muted">
  The z coordinate, in the coordinate system of the printer, the extruded filament touches a colony on the source plate, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>z-coordinate Master:</label>
  <input id="z_coordinate_extruded_filament_inside_master_plate" name="z_coordinate_extruded_filament_inside_master_plate" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${printer_profile.z_coordinate_extruded_filament_inside_master_plate}">
  <small class="form-text text-muted">
  The z coordinate, in the coordinate system of the printer, the extruded filament touches the culture medium on the master plate, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>z-coordinate Goal:</label>
  <input id="z_coordinate_extruded_filament_inside_goal_plate" name="z_coordinate_extruded_filament_inside_goal_plate" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${printer_profile.z_coordinate_extruded_filament_inside_goal_plate}">
  <small class="form-text text-muted">
  The z coordinate, in the coordinate system of the printer, the extruded filament touches the liqour inside the well of the goal plate, in millimeter.
  </small>
  </div>
  <button type="submit" class="btn btn-dark">${button_text}</button>
  </form>
  </div>
  </div>
  </div>
  `;

  document.getElementById('printer-profiles').insertAdjacentHTML('beforeend',html);
  $('#form-'+printer_profile.id).on('submit', UpdateSettingsProfile);

  if(printer_profile.profile_name != ""){
    let printer_profile_option = document.createElement('option');
    printer_profile_option.value = printer_profile.id;
    printer_profile_option.text = printer_profile.profile_name;
    document.getElementById("select-printer-profile").appendChild(printer_profile_option);
  }
}

function AddSocketProfile(socket_profile){
  socket_profiles[socket_profile.id] = socket_profile;

  let link_text = socket_profile.profile_name;
  let button_text = "Save changes";
  let deletable = true;

  if(socket_profile.id == "newsocket-profile"){
    link_text = "Create new socket profile ->"; 
    button_text = "Create socket profile";
    deletable = false;
  } 

  const html = `
  <div class="card" id="card-${socket_profile.id}">
  <div class="card-header">
  <h2 class="mb-0">
  ${deletable ? `<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="socket-profile" data-id="${socket_profile.id}">&times;</button>` : ``}
  <button id="link-${socket_profile.id}" class="btn btn-link collapsed" type="button" data-toggle="collapse" data-target="#collapse-${socket_profile.id}">
  ${link_text}
  </button>
  </h2>
  </div>
  <div id="collapse-${socket_profile.id}" class="collapse" data-parent="#group">
  <div class="card-body">
  <form id="form-${socket_profile.id}" name="form-${socket_profile.id}" method="post" enctype="multipart/form-data">
  <input type="hidden" id="id" name="id" value="${socket_profile.id}">
  <input type="hidden" id="type" name="type" value="socket-profile">
  <div class="form-group">
  <label for="profile_name">Name:</label>
  <input id="profile_name" class="form-control" name="profile_name" type="text" placeholder="name" value="${socket_profile.profile_name}" required="required">
  </div>
  <div class="form-group">
  <label>Source plate origin:</label>
  
  (<input id="global_origin_of_source_plate_x" class="d-inline form-control-plaintext" name="global_origin_of_source_plate_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.global_origin_of_source_plate_x}">,
  <input id="global_origin_of_source_plate_y" name="global_origin_of_source_plate_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.global_origin_of_source_plate_y}">)
  <small class="form-text text-muted">
  The origin of the slot/cut-out of the source plate given as a point of the coordinate system of the socket
  </small>
  </div>
  <div class="form-group">
  <label>Master plate origin:</label>
  (<input id="global_origin_of_master_plate_x" name="global_origin_of_master_plate_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.global_origin_of_master_plate_x}">,
  <input id="global_origin_of_master_plate_y" name="global_origin_of_master_plate_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.global_origin_of_master_plate_y}">)
  <small class="form-text text-muted">
  The origin of the slot/cut-out of the master plate given as a point of the coordinate system of the socket
  </small>
  </div>
  <div class="form-group">
  <label>Goal plate origin:</label>
  (<input id="global_origin_of_goal_plate_x" name="global_origin_of_goal_plate_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.global_origin_of_goal_plate_x}">,
  <input id="global_origin_of_goal_plate_y" name="global_origin_of_goal_plate_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.global_origin_of_goal_plate_y}">)
  <small class="form-text text-muted">
  The origin of the slot/cut-out of the goal plate given as a point of the coordinate system of the socket
  </small>
  </div>
  <div class="form-group">
  <label>Goal and master plate orientation:</label>
  <div class="form-check form-check-inline">
  <input class="form-check-input" type="radio" name="orientation_of_goal_plate" id="kFirstRowFirstColumnAtCutoutOrigin" value="kFirstRowFirstColumnAtCutoutOrigin" checked>
  <label class="form-check-label" for="kFirstRowFirstColumnAtCutoutOrigin">kFirstRowFirstColumnAtCutoutOrigin</label>
  </div>
  <div class="form-check form-check-inline">
  <input class="form-check-input" type="radio" name="orientation_of_goal_plate" id="kLastRowFirstColumnAtCutoutOrigin" value="kLastRowFirstColumnAtCutoutOrigin">
  <label class="form-check-label" for="kLastRowFirstColumnAtCutoutOrigin">kLastRowFirstColumnAtCutoutOrigin</label>
  </div>
  <small class="form-text text-muted">
  The orientation of the goal plate compared to the cutout it is lying in.
  </small>
  </div>
  <div class="form-group">
  <label>Socket origin offset:</label>
  (<input id="socket_origin_offset_x" name="socket_origin_offset_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.socket_origin_offset_x}">,
  <input id="socket_origin_offset_y" name="socket_origin_offset_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${socket_profile.socket_origin_offset_y}">)
  <small class="form-text text-muted">
  The offset of the origin of the socket to the origin of the printers coordinate system in y direction
  </small>
  </div>
  <button type="submit" class="btn btn-dark">${button_text}</button>
  </form>
  </div>
  </div>
  </div>
  `;

  document.getElementById('socket-profiles').insertAdjacentHTML('beforeend',html);
  $('#form-'+socket_profile.id).on('submit', UpdateSettingsProfile);
  
  if(socket_profile.profile_name != ""){
    let socket_profile_option = document.createElement('option');
    socket_profile_option.value = socket_profile.id;
    socket_profile_option.text = socket_profile.profile_name;
    document.getElementById("select-socket-profile").appendChild(socket_profile_option);
  }
}


function AddPlateProfile(plate_profile){
  plate_profiles[plate_profile.id] = plate_profile;

  let link_text = plate_profile.profile_name;
  let button_text = "Save changes";
  let deletable = true;

  if(plate_profile.id == "newplate-profile"){
    link_text = "Create new goal plate profile ->"; 
    button_text = "Create plate profile";
    deletable = false;
  } 

  const html = `
  <div class="card" id="card-${plate_profile.id}">
  <div class="card-header">
  <h2 class="mb-0">
  ${deletable ? `<button type="button" class="close" data-toggle="modal" data-target="#delete-dialog" data-type="plate-profile" data-id="${plate_profile.id}">&times;</button>` : ``}
  <button id="link-${plate_profile.id}" class="btn btn-link collapsed" type="button" data-toggle="collapse" data-target="#collapse-${plate_profile.id}">
  ${link_text}
  </button>
  </h2>
  </div>
  <div id="collapse-${plate_profile.id}" class="collapse" data-parent="#group">
  <div class="card-body">
  <form id="form-${plate_profile.id}" name="form-${plate_profile.id}" method="post" enctype="multipart/form-data">
  <input type="hidden" id="id" name="id" value="${plate_profile.id}">
  <input type="hidden" id="type" name="type" value="plate-profile">
  <div class="form-group">
  <label for="profile_name">Name:</label>
  <input id="profile_name" class="form-control" name="profile_name" type="text" placeholder="name" value="${plate_profile.profile_name}" required="required">
  </div>
  <div class="form-group">
  <label>Number of rows:</label>
  <input id="number_of_rows" name="number_of_rows" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="1" value="${plate_profile.number_of_rows}">
  <small class="form-text text-muted">
  Number of rows the goal plate has.
  </small>
  </div>
  <div class="form-group">
  <label>Number of columns:</label>
  <input id="number_of_columns" name="number_of_columns" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="1" value="${plate_profile.number_of_columns}">
  <small class="form-text text-muted">
  Number of columns the goal plate has.
  </small>
  </div>
  <div class="form-group">
  <label>A1 row offset:</label>
  <input id="a1_row_offset" name="a1_row_offset" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${plate_profile.a1_row_offset}">
  <small class="form-text text-muted">
  The offset of the center of the first well A1 to the upper edge of the goal plate.
  </small>
  </div>
  <div class="form-group">
  <label>A1 column offset:</label>
  <input id="a1_column_offset" name="a1_column_offset" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${plate_profile.a1_column_offset}">
  <small class="form-text text-muted">
  The offset of the center of the first well A1 to the left edge of the goal plate.
  </small>
  </div><div class="form-group">
  <label>Well spacing center to center:</label>
  <input id="well_spacing_center_to_center" name="well_spacing_center_to_center" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.1" value="${plate_profile.well_spacing_center_to_center}">
  <small class="form-text text-muted">
  The distance between the center of a well to the center of any directly adjacent wells.
  </small>
  </div>
  <button type="submit" class="btn btn-dark">${button_text}</button>
  </form>
  </div>
  </div>
  </div>
  `;

  document.getElementById('plate-profiles').insertAdjacentHTML('beforeend',html);
  $('#form-'+plate_profile.id).on('submit', UpdateSettingsProfile);
}