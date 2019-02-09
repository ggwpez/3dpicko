var UpdateSettingsProfile = function (e){
  e.preventDefault();
  
  const form_data = new FormData(this);
  var json_object = {};
  for (const [key, value]  of form_data.entries()) {
    json_object[key] = value;
  }
  if(json_object.id == "newprinter-profile" || json_object.id == "newsocket-profile" || json_object.id == "newplate-profile"){
    $('#form-new'+json_object.type)[0].reset();
    $('#collapse-new'+json_object.type).collapse('hide');
    api("createsettingsprofile", json_object);
  } 
  else{
    $('#collapse-'+json_object.id).collapse('hide');
    api("updatesettingsprofile", json_object);
  } 
}

$(function LoadProfiles(){
  // add empty "newprofiles", initial values possible
  AddPrinterProfile({
    "id" : "newprinter-profile",
    "type" : "printer-profile",
    "profile_name" : "",
    "cut_filament_position_above_trigger_x" : "",
    "cut_filament_position_above_trigger_y" : "",
    "cut_filament_position_above_trigger_z" : "",
    "z_coordinate_pushing_the_trigger" : "",
    "distance_between_pushed_trigger_and_gap_between_scissors_blade" : "",
    "movement_speed" : 0,
    "filament_extrusion_length_on_move_offset" : 0,
    "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : 0
  });
  AddSocketProfile({
    "id" : "newsocket-profile",
    "type" : "socket-profile",
    "socket_origin_offset_x" : "",
    "socket_origin_offset_y" : "",
    "socket_origin_offset_z" : "",
    "profile_name" : "",
    "global_origin_of_source_plate_x" : "",
    "global_origin_of_source_plate_y" : "",
    "depth_of_cutout_the_source_plate_lies_in" : "",
    "global_origin_of_master_plate_x" : "",
    "global_origin_of_master_plate_y" : "",
    "depth_of_cutout_the_master_plate_lies_in" : "",
    "global_origin_of_goal_plate_x" : "",
    "global_origin_of_goal_plate_y" : "",
    "depth_of_cutout_the_goal_plate_lies_in" : "",
    "orientation_of_goal_plate" : "kFirstRowFirstColumnAtCutoutOrigin"
  });
  AddPlateProfile({
    "id" : "newplate-profile",
    "type" : "plate-profile",
    "profile_name" : "",
    "number_of_rows" : "",
    "number_of_columns" : "",
    "a1_row_offset" : "",
    "a1_column_offset" : "",
    "well_spacing_center_to_center" : "",
    "height_source_plate" : "",
    "height_master_plate" : "",
    "height_goal_plate" : "",
    "well_depth" : "",
    "culture_medium_thickness" : ""
  });

  // local profiles for debugging
  const prototyp1 = {
    "id" : "DEFAULTBLUE",
    "type" : "socket-profile",
    "socket_origin_offset_x" : "1",
    "socket_origin_offset_y" : "2",
    "socket_origin_offset_z" : "3",
    "profile_name" : "Default Socket Blue",
    "global_origin_of_source_plate_x" : "4",
    "global_origin_of_source_plate_y" : "5",
    "depth_of_cutout_the_source_plate_lies_in" : "6",
    "global_origin_of_master_plate_x" : "7",
    "global_origin_of_master_plate_y" : "8",
    "depth_of_cutout_the_master_plate_lies_in" : "9",
    "global_origin_of_goal_plate_x" : "10",
    "global_origin_of_goal_plate_y" : "11",
    "depth_of_cutout_the_goal_plate_lies_in" : "12",
    "orientation_of_goal_plate" : "kFirstRowFirstColumnAtCutoutOrigin"
  };
  AddProfileToList(prototyp1);
  const default_plate = {
    "id" : "DEFALUTPLATE",
    "type" : "plate-profile",
    "profile_name" : "Default Plates - 96 Wells",
    "number_of_rows" : "8",
    "number_of_columns" : "12",
    "a1_row_offset" : "1",
    "a1_column_offset" : "2",
    "well_spacing_center_to_center" : "3",
    "height_source_plate" : "4",
    "height_master_plate" : "5",
    "height_goal_plate" : "6",
    "well_depth" : "7",
    "culture_medium_thickness" : "8"
  }
  AddProfileToList(default_plate);
  const makergear = {
    "id" : "XYZ",
    "type" : "printer-profile",
    "profile_name" : "Makergear M3",
    "cut_filament_position_above_trigger_x" : "1",
    "cut_filament_position_above_trigger_y" : "2",
    "cut_filament_position_above_trigger_z" : "3",
    "z_coordinate_pushing_the_trigger" : "4",
    "distance_between_pushed_trigger_and_gap_between_scissors_blade" : "5",
    "movement_speed" : "6",
    "filament_extrusion_length_on_move_offset": "7",
    "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : "8"
  };
  const creality = {
    "id" : "ABC",
    "type" : "printer-profile",
    "profile_name" : "Creality Ender-3",
    "cut_filament_position_above_trigger_x" : "1",
    "cut_filament_position_above_trigger_y" : "2",
    "cut_filament_position_above_trigger_z" : "3",
    "z_coordinate_pushing_the_trigger" : "4",
    "distance_between_pushed_trigger_and_gap_between_scissors_blade" : "5",
    "movement_speed" : "6",
    "filament_extrusion_length_on_move_offset": "7",
    "filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" : "8"
  };
  AddProfileToList(makergear);
  AddProfileToList(creality);  
});

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

function AddProfileToList(profile){
  if(profile.id in all_profiles){
    // update profile (delete old profile)
    DeleteProfile(profile.id);
  }
  // add new profile
  all_profiles[profile.id] = profile;
  switch(profile.type){
    case "printer-profile": 
    AddPrinterProfile(profile);
    break;
    case "socket-profile": 
    AddSocketProfile(profile);
    break;
    case "plate-profile": 
    AddPlateProfile(profile);
    break;
  }
}

function AddPrinterProfile(printer_profile){
  console.log("Profile:", printer_profile);
  let link_text = printer_profile.profile_name;
  let button_text = "Save changes";
  let deletable = true;
 

  if(printer_profile.id == "newprinter-profile"){
    link_text = "Create new printer profile >"; 
    button_text = "Create profile";
    deletable = false;
  }
  // TODO
  if(!("filament_extrusion_length_on_move_offset" in printer_profile)){
    printer_profile.filament_extrusion_length_on_move_offset = printer_profile.filament_extrusion_length_on_move - printer_profile.filament_extrusion_length_default;
    printer_profile.filament_extrusion_length_on_pick_and_put_onto_master_plate_offset = printer_profile.filament_extrusion_length_on_pick_and_put_onto_master_plate - printer_profile.filament_extrusion_length_default;
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
  <label for="profile_name">Printer profile identifier:</label>
  <input required="required" id="profile_name" class="form-control" name="profile_name" type="text" placeholder="choose identifier" value="${printer_profile.profile_name}">
  </div>
  <div class="form-group">
  <label>Cut position: </label>
  (<input required="required" id="cut_filament_position_above_trigger_x" name="cut_filament_position_above_trigger_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" min="0" step="0.01" value="${printer_profile.cut_filament_position_above_trigger_x}">,
  <input required="required" id="cut_filament_position_above_trigger_y" name="cut_filament_position_above_trigger_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${printer_profile.cut_filament_position_above_trigger_y}">,
  <input required="required" id="cut_filament_position_above_trigger_z" name="cut_filament_position_above_trigger_z" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${printer_profile.cut_filament_position_above_trigger_z}">) mm.
  <small class="form-text text-muted">
  The (x,y,z) position, in the coordinate system of the printer, the nozzle needs to move to in order to be above the trigger of the scissor and directly above the center of the space between the scissors blades.
  </small>
  </div>
  <div class="form-group">
  <label>Push trigger at:</label>
  <input required="required" id="z_coordinate_pushing_the_trigger" name="z_coordinate_pushing_the_trigger" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${printer_profile.z_coordinate_pushing_the_trigger}"> mm.
  <small class="form-text text-muted">
  The global z coordinate (at x and y of cut-position) the trigger of the scissor is definitely pushed.
  </small>
  </div>
  <div class="form-group">
  <label>Cut distance:</label>
  <input required="required" id="distance_between_pushed_trigger_and_gap_between_scissors_blade" name="distance_between_pushed_trigger_and_gap_between_scissors_blade" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${printer_profile.distance_between_pushed_trigger_and_gap_between_scissors_blade}"> mm.
  <small class="form-text text-muted">
  The distance between the nozzle when the trigger is pushed and the gap between the scissors blades, where the filament will be cut.
  </small>
  </div>
  <div class="form-group">
  <label>Movement speed:</label>
  <input required="required" id="movement_speed" name="movement_speed" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="1" value="${printer_profile.movement_speed}"> mm/min.
  <small class="form-text text-muted">
  The speed the nozzle is moved with.
  </small>
  </div>
  <div class="form-group">
  <label>Filament offset (move):</label>
  <input required="required" id="filament_extrusion_length_on_move_offset" name="filament_extrusion_length_on_move_offset" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${printer_profile.filament_extrusion_length_on_move_offset}"> mm.
  <small class="form-text text-muted">
  Offset to the length up to which the filament will be extruded while moving the nozzle above the plates.
  </small>
  </div>
  <div class="form-group">
  <label>Filament offset (pick):</label>
  <input required="required" id="filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" name="filament_extrusion_length_on_pick_and_put_onto_master_plate_offset" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${printer_profile.filament_extrusion_length_on_pick_and_put_onto_master_plate_offset}"> mm.
  <small class="form-text text-muted">
  Offset to the length up to which the filament will be extruded when picking from source- and placing on masterplate.
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
  $('#form-'+printer_profile.id).on('focus', 'input', function(){this.select();});

  if(printer_profile.profile_name != ""){
    let printer_profile_option = document.createElement('option');
    printer_profile_option.value = printer_profile.id;
    printer_profile_option.text = printer_profile.profile_name;
    document.getElementById("select-printer-profile").appendChild(printer_profile_option);
  }
}

function AddSocketProfile(socket_profile){
  let link_text = socket_profile.profile_name;
  let button_text = "Save changes";
  let deletable = true;

  if(socket_profile.id == "newsocket-profile"){
    link_text = "Create new socket profile >"; 
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
  <label for="profile_name">Socket profile identifier:</label>
  <input required="required" id="profile_name" class="form-control" name="profile_name" type="text" placeholder="choose identifier" value="${socket_profile.profile_name}">
  </div>
  <div class="form-group">
  <label>Socket origin offset:</label>
  (<input onclick="this.select();" required="required" id="socket_origin_offset_x" name="socket_origin_offset_x" type="number" class="d-inline form-control-plaintext" placeholder="0" step="0.01" value="${socket_profile.socket_origin_offset_x}">,
  <input required="required" id="socket_origin_offset_y" name="socket_origin_offset_y" type="number" class="d-inline form-control-plaintext" placeholder="0" step="0.01" value="${socket_profile.socket_origin_offset_y}">,
  <input required="required" id="socket_origin_offset_z" name="socket_origin_offset_z" type="number" class="d-inline form-control-plaintext" placeholder="0" step="0.01" value="${socket_profile.socket_origin_offset_z}">) mm.
  <small class="form-text text-muted">
  The (x,y,z) offset of the socket origin (with z-origin on socket surface) to the origin of the printers coordinate system (home).
  </small>
  </div>
  <div class="form-group">
  <label>Source plate cutout origin: </label> 
  (<input required="required" id="global_origin_of_source_plate_x" class="d-inline form-control-plaintext" name="global_origin_of_source_plate_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.global_origin_of_source_plate_x}">,
  <input required="required" id="global_origin_of_source_plate_y" name="global_origin_of_source_plate_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.global_origin_of_source_plate_y}">) mm.
  <br>
  <label>Source plate cutout depth:</label>
  <input required="required" id="depth_of_cutout_the_source_plate_lies_in" class="d-inline form-control-plaintext" name="depth_of_cutout_the_source_plate_lies_in" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.depth_of_cutout_the_source_plate_lies_in}"> mm.
  <small class="form-text text-muted">
  The (x,y) origin of the slot/cut-out of the source plate given as a point of the coordinate system of the socket and its depth.
  </small>
  </div>
  <div class="form-group">
  <label>Master plate cutout origin:</label>
  (<input required="required" id="global_origin_of_master_plate_x" name="global_origin_of_master_plate_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.global_origin_of_master_plate_x}">,
  <input required="required" id="global_origin_of_master_plate_y" name="global_origin_of_master_plate_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.global_origin_of_master_plate_y}">) mm.
  <br>
  <label>Master plate cutout depth:</label>
  <input required="required" id="depth_of_cutout_the_master_plate_lies_in" class="d-inline form-control-plaintext" name="depth_of_cutout_the_master_plate_lies_in" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.depth_of_cutout_the_master_plate_lies_in}"> mm.
  <small class="form-text text-muted">
  The (x,y) origin of the slot/cut-out of the master plate given as a point of the coordinate system of the socket and its depth.
  </small>
  </div>
  <div class="form-group">
  <label>Goal plate cutout origin:</label>
  (<input required="required" id="global_origin_of_goal_plate_x" name="global_origin_of_goal_plate_x" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.global_origin_of_goal_plate_x}">,
  <input required="required" id="global_origin_of_goal_plate_y" name="global_origin_of_goal_plate_y" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.global_origin_of_goal_plate_y}">) mm.
  <br>
  <label>Goal plate cutout depth:</label>
  <input required="required" id="depth_of_cutout_the_goal_plate_lies_in" class="d-inline form-control-plaintext" name="depth_of_cutout_the_goal_plate_lies_in" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${socket_profile.depth_of_cutout_the_goal_plate_lies_in}"> mm.
  <small class="form-text text-muted">
  The (x,y) origin of the slot/cut-out of the goal plate given as a point of the coordinate system of the socket and its depth.
  </small>
  </div>
  <div class="form-group">
  <label>Goal and master plate orientation:</label><br>
  <div class="form-check form-check-inline">
  <input class="form-check-input" type="radio" name="orientation_of_goal_plate" id="kFirstRowFirstColumnAtCutoutOrigin" value="kFirstRowFirstColumnAtCutoutOrigin" ${(socket_profile.orientation_of_goal_plate=="kFirstRowFirstColumnAtCutoutOrigin")?`checked`:``}>
  <label class="form-check-label font-weight-normal" for="kFirstRowFirstColumnAtCutoutOrigin">Well "A1" at cutout origin.</label>
  </div><br>
  <div class="form-check form-check-inline">
  <input class="form-check-input" type="radio" name="orientation_of_goal_plate" id="kLastRowFirstColumnAtCutoutOrigin" value="kLastRowFirstColumnAtCutoutOrigin" ${(socket_profile.orientation_of_goal_plate=="kLastRowFirstColumnAtCutoutOrigin")?`checked`:``}>
  <label class="form-check-label font-weight-normal" for="kLastRowFirstColumnAtCutoutOrigin">Well "m1" at cutout origin. (m = last row)</label>
  </div>
  <small class="form-text text-muted mt-1">
  Orientation of goal- and masterplate compared to the cutout it is lying in.
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
  $('#form-'+socket_profile.id).on('focus', 'input', function(){this.select();});

  if(socket_profile.profile_name != ""){
    let socket_profile_option = document.createElement('option');
    socket_profile_option.value = socket_profile.id;
    socket_profile_option.text = socket_profile.profile_name;
    document.getElementById("select-socket-profile").appendChild(socket_profile_option);
  }
}


function AddPlateProfile(plate_profile){
  let link_text = plate_profile.profile_name;
  let button_text = "Save changes";
  let deletable = true;

  if(plate_profile.id == "newplate-profile"){
    link_text = "Create new goal plate profile >"; 
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
  <label for="profile_name">Plate profile identifier:</label>
  <input required="required" id="profile_name" class="form-control" name="profile_name" type="text" placeholder="choose identifier" value="${plate_profile.profile_name}">
  </div>
  <div class="form-group">
  <label>Number of rows:</label>
  <input required="required" id="number_of_rows" name="number_of_rows" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="1" value="${plate_profile.number_of_rows}"> rows.
  <small class="form-text text-muted">
  Number of rows the goal plate has.
  </small>
  </div>
  <div class="form-group">
  <label>Number of columns:</label>
  <input required="required" id="number_of_columns" name="number_of_columns" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="1" value="${plate_profile.number_of_columns}"> columns.
  <small class="form-text text-muted">
  Number of columns the goal plate has.
  </small>
  </div>
  <div class="form-group">
  <label>A1 row offset:</label>
  <input required="required" id="a1_row_offset" name="a1_row_offset" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.a1_row_offset}"> mm.
  <small class="form-text text-muted">
  The offset of the center of the first well A1 to the upper edge of the goal plate.
  </small>
  </div>
  <div class="form-group">
  <label>A1 column offset:</label>
  <input required="required" id="a1_column_offset" name="a1_column_offset" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.a1_column_offset}"> mm.
  <small class="form-text text-muted">
  The offset of the center of the first well A1 to the left edge of the goal plate.
  </small>
  </div>
  <div class="form-group">
  <label>Well spacing center to center:</label>
  <input required="required" id="well_spacing_center_to_center" name="well_spacing_center_to_center" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.well_spacing_center_to_center}"> mm.
  <small class="form-text text-muted">
  The distance between the center of a well to the center of any directly adjacent well.
  </small>
  </div>
  <div class="form-group">
  <label>Source plate height:</label>
  <input required="required" id="height_source_plate" name="height_source_plate" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.height_source_plate}"> mm.
  <small class="form-text text-muted">
  The height of the source plate.
  </small>
  </div>
  <div class="form-group">
  <label>Master plate height:</label>
  <input required="required" id="height_master_plate" name="height_master_plate" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.height_master_plate}"> mm.
  <small class="form-text text-muted">
  The height of the master plate.
  </small>
  </div>
  <div class="form-group">
  <label>Goal plate height:</label>
  <input required="required" id="height_goal_plate" name="height_goal_plate" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.height_goal_plate}"> mm.
  <small class="form-text text-muted">
  The height of the goal plate.
  </small>
  </div>
  <div class="form-group">
  <label>Well depth:</label>
  <input required="required" id="well_depth" name="well_depth" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.well_depth}"> mm.
  <small class="form-text text-muted">
  The depth of every well.
  </small>
  </div>
  <div class="form-group">
  <label>Culture medium thickness:</label>
  <input required="required" id="culture_medium_thickness" name="culture_medium_thickness" type="number" class="d-inline form-control-plaintext" placeholder="0" min="0" step="0.01" value="${plate_profile.culture_medium_thickness}"> mm.
  <small class="form-text text-muted">
  The thickness of the used culture medium inside the source and master plate, for instance agars.
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
  $('#form-'+plate_profile.id).on('focus', 'input', function(){this.select();});
}