/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "GUI/api_commands.h"

namespace d3picko {
QString APICommands::GET_IMAGE_LIST = "getimagelist";
QString APICommands::GET_PROFILE_LIST = "getprofilelist";
QString APICommands::GET_VERSION_LIST = "getversionlist";
QString APICommands::GET_JOB_LIST = "getjoblist";
QString APICommands::VERSION_SWITCHED = "versionswitched";
QString APICommands::GET_DETECTION_ALGORITHMS = "getdetectionalgorithms";
QString APICommands::SET_DEFAULT_SETTINGS_PROFILE = "setdefaultsettingsprofile";
QString APICommands::SET_STARTING_WELL = "setstartingwell";
QString APICommands::DELETE_IMAGE = "deleteimage";
QString APICommands::DELETE_JOB = "deletejob";
QString APICommands::START_JOB = "startjob";
QString APICommands::GET_REPORT = "getreport";
QString APICommands::UPLOAD_IMAGE = "uploadimage";
QString APICommands::UPDATE_SOFTWARE = "updatesoftware";
QString APICommands::CREATE_SETTINGS_PROFILE = "createsettingsprofile";
QString APICommands::UPDATE_SETTINGS_PROFILE = "updatesettingsprofile";
QString APICommands::DELETE_SETTINGS_PROFILE = "deletesettingsprofile";
QString APICommands::CREATE_JOB = "createjob";
QString APICommands::GET_POSITIONS = "getpositions";
QString APICommands::SET_COLONIES_TO_PICK = "setcoloniestopick";
QString APICommands::UPDATE_DETECTION_SETTINGS = "updatedetectionsettings";
QString APICommands::SHUTDOWN = "shutdown";
QString APICommands::RESTART = "restart";
QString APICommands::BACKUP = "backup";
}  // namespace d3picko
