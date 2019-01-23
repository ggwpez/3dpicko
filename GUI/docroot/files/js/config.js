var printer_profiles = {};
var ws;

(function WsSetup() {
  ws = new WebSocket("ws://" +window.location.hostname +":8888/");

  ws.onmessage = function(evt)
  {
    var msg = JSON.parse(evt.data);
    console.log("received data:", msg.data);

    if (msg.type == "createsettingsprofile"){
      const profile_object = msg.data; 
      switch(profile_object.type){
        case "printer-profile": 
        AddPrinterProfile(profile_object);
        printer_profiles[profile_object.id] = profile_object;
        break;
      }
      $('#form-new'+profile_object.type)[0].reset();
      $('#collapse-new'+profile_object.type).collapse('hide');
      $('#info-dialog-title').text(profile_object.profile_name+" erfolgreich hinzugefügt.");
      $('#info-dialog').modal('show');
    }
    else if(msg.type == "updatesettingsprofile"){
      const profile_object = msg.data; 
      switch(profile_object.type){
        case "printer-profile": 
        printer_profiles[profile_object.id] = profile_object;
        break;
      }
      $('#link-'+profile_object.id).text(profile_object.profile_name);
      $('#collapse-'+profile_object.id).collapse('hide');
      $('#info-dialog-title').text(profile_object.profile_name+" erfolgreich aktualisiert.");
      $('#info-dialog').modal('show');
    }
    else
    {
      alert("Unknown message type " +msg.type);
    }
  };
  ws.onerror = function (errorEvent)
  {
    alert("Error! Die Verbindung wurde unerwartet geschlossen");
  };
})();

function api(query, data)
{
  console.log("send data:", data);
  ws.send(JSON.stringify({request: /*"/apiv2/" +*/query, data: data}));
}

var UpdateSettingsProfile = function (e){
  e.preventDefault();
  
  const form_data = new FormData(this);
  var json_object = {};
  for (const [key, value]  of form_data.entries()) {
    json_object[key] = value;
  }
  if(json_object.id == "newprinter-profile") api("createsettingsprofile", json_object);
  else api("updatesettingsprofile", json_object);
}

function DeletePrinterProfile(id){
  $('#card-'+id).remove();
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
    document.getElementById('delete-dialog-body').innerHTML = `Druckerprofil ${printer_profiles[id].profile_name} löschen?`;
    document.getElementById('delete-button').onclick = function(){DeletePrinterProfile(id)};
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
  let button_text = "Änderungen speichern";
  let deletable = true;

  if(printer_profile.id == "newprinter-profile"){
    link_text = "Neues Druckerprofil anlegen ->"; 
    button_text = "Druckerprofil anlegen";
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
  <div id="collapse-${printer_profile.id}" class="collapse" data-parent="#printer-profiles">
  <div class="card-body">
  <form id="form-${printer_profile.id}" name="form-${printer_profile.id}" method="post" enctype="multipart/form-data">
  <input type="hidden" id="id" name="id" value="${printer_profile.id}">
  <input type="hidden" id="type" name="type" value="printer-profile">
  <div class="form-group">
  <label for="profile_name">Bezeichnung für das Druckerprofil:</label>
  <input id="profile_name" class="form-control" name="profile_name" type="text" placeholder="Bezeichnung" value="${printer_profile.profile_name}" required="required">
  </div>
  <div class="form-group">
  <label>Cut-Position:</label>
  (<input id="cut_filament_position_x" name="cut_filament_position_x" type="number" placeholder="x" step="1" value="${printer_profile.cut_filament_position_x}">/
  <input id="cut_filament_position_y" name="cut_filament_position_y" type="number" placeholder="y" step="1" value="${printer_profile.cut_filament_position_y}">)
  <small class="form-text text-muted">
  X/Y Postion, im Koordinatensystem des Druckers, in den der Druckkopf fahren muss, um das Filament abschneiden zu lassen.
  </small>
  </div>
  <div class="form-group">
  <label>Filament-Länge:</label>
  <input id="filament_extrusion_length" name="filament_extrusion_length" type="number" placeholder="1" step="1" value="${printer_profile.filament_extrusion_length}">
  <small class="form-text text-muted">
  The length up to which the filament shall be extruded, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>Z-Koordinate:</label>
  <input id="z_coordinate_extruded_filament_above_plates" name="z_coordinate_extruded_filament_above_plates" type="number" placeholder="z" step="1" value="${printer_profile.z_coordinate_extruded_filament_above_plates}">
  <small class="form-text text-muted">
  The z coordinate, in the coordinate system of the printer, the extruded filament can be moved above the plates without touching them or anything else, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>Z-Koordinate Source:</label>
  <input id="z_coordinate_extruded_filament_inside_source_plate" name="z_coordinate_extruded_filament_inside_source_plate" type="number" placeholder="z" step="1" value="${printer_profile.z_coordinate_extruded_filament_inside_source_plate}">
  <small class="form-text text-muted">
  The z coordinate, in the coordinate system of the printer, the extruded filament touches a colony on the source plate, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>Z-Koordinate Master:</label>
  <input id="z_coordinate_extruded_filament_inside_master_plate" name="z_coordinate_extruded_filament_inside_master_plate" type="number" placeholder="z" step="1" value="${printer_profile.z_coordinate_extruded_filament_inside_master_plate}">
  <small class="form-text text-muted">
  The z coordinate, in the coordinate system of the printer, the extruded filament touches the culture medium on the master plate, in millimeter.
  </small>
  </div>
  <div class="form-group">
  <label>Z-Koordinate Goal:</label>
  <input id="z_coordinate_extruded_filament_inside_goal_plate" name="z_coordinate_extruded_filament_inside_goal_plate" type="number" placeholder="z" step="1" value="${printer_profile.z_coordinate_extruded_filament_inside_goal_plate}">
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
}