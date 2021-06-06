/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#include "CLI.h"

#include <LittleFS.h>
#include "functions.h"
// #include "settings.h"
// #include "wifi.h"

/*
   Shitty code used less resources so I will keep this clusterfuck as it is,
   but if you're interested I made a library for this: github.com/spacehuhn/SimpleCLI
 */

CLI::CLI() {
    list  = new SimpleList<String>;
    queue = new SimpleList<String>;
}

CLI::~CLI() {}

void CLI::load() {
    String defaultValue = str(CLI_DEFAULT_AUTOSTART);

    checkFile(execPath, defaultValue);
    execFile(execPath);
}

void CLI::load(String filepath) {
    execPath = filepath;
    load();
}

void CLI::enable() {
    enabled = true;
    prntln(CLI_SERIAL_ENABLED);
}

void CLI::disable() {
    enabled = true;
    prntln(CLI_SERIAL_DISABLED);
}

void CLI::update() {
    // when serial available, read input
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        exec(input);
    }

    // when queue is not empty, delay is off and no scan is active, run it
    // else if ((queue->size() > 0) && !delayed && !scan.isScanning() && !attack.isRunning()) {
    else if (queue->size() > 0 && !delayed ) {
        String s = queue->shift();
        exec(s);
    }
}

void CLI::stop() {
    queue->clear();
    prntln(CLI_STOPPED_SCRIPT);
}

void CLI::enableDelay(uint32_t delayTime) {
    delayed         = true;
    this->delayTime = delayTime;
    delayStartTime  = millis();
}

void CLI::exec(String input) {
    // quick exit when input is empty
    if (input.length() == 0) return;

    // check delay
    if (delayed && (millis() - delayStartTime > delayTime)) {
        delayed = false;
        prntln(CLI_RESUMED);
    }

    // when delay is on, add it to queue, else run it
    if (delayed) {
        queue->add(input);
    } else {
        runLine(input);
    }
}

void CLI::execFile(String path) {
    String input;

    if (readFile(path, input)) {
        String tmpLine;
        char   tmpChar;

        input += '\n';

        while (!queue->isEmpty()) {
            input += queue->shift();
            input += '\n';
        }

        for (int i = 0; i < input.length(); i++) {
            tmpChar = input.charAt(i);

            if (tmpChar == '\n') {
                queue->add(tmpLine);
                tmpLine = String();
            } else {
                tmpLine += tmpChar;
            }
        }

        queue->add(tmpLine);
    }
}

void CLI::error(String message) {
    prnt(CLI_ERROR);
    prntln(message);
}

void CLI::parameterError(String parameter) {
    prnt(CLI_ERROR_PARAMETER);
    prnt(parameter);
    prntln(DOUBLEQUOTES);
}

bool CLI::isInt(String str) {
    if (eqls(str, STR_TRUE) || eqls(str, STR_FALSE)) return true;

    for (uint32_t i = 0; i < str.length(); i++)
        if (!isDigit(str.charAt(i))) return false;

    return true;
}

int CLI::toInt(String str) {
    if (eqls(str, STR_TRUE)) return 1;
    else if (eqls(str, STR_FALSE)) return 0;
    else return str.toInt();
}

uint32_t CLI::getTime(String time) {
    int value = time.toInt();

    if (value < 0) value = -value;

    if (time.substring(time.length() - 1).equalsIgnoreCase(String(S))) value *= 1000;
    else if (time.substring(time.length() - 3).equalsIgnoreCase(str(STR_MIN)) ||
             (time.charAt(time.length() - 1) == M)) value *= 60000;
    return value;
}

bool CLI::eqlsCMD(int i, const char* keyword) {
    return eqls(list->get(i).c_str(), keyword);
}

void CLI::runLine(String input) {
    String tmp;

    for (int i = 0; i < input.length(); i++) {
        // when 2 semicolons in a row without a backslash escaping the first
        if ((input.charAt(i) == SEMICOLON) && (input.charAt(i + 1) == SEMICOLON) &&
            (input.charAt(i - 1) != BACKSLASH)) {
            runCommand(tmp);
            tmp = String();
            i++;
        } else {
            tmp += input.charAt(i);
        }
    }

    tmp.replace(BACKSLASH + SEMICOLON + SEMICOLON, SEMICOLON + SEMICOLON);

    if (tmp.length() > 0) runCommand(tmp);
}

void CLI::runCommand(String input) {
    input.replace(String(NEWLINE), String());
    input.replace(String(CARRIAGERETURN), String());

    list->clear();

    // parse/split input in list
    String tmp;
    bool   withinQuotes = false;
    bool   escaped      = false;
    char   c;

    for (uint32_t i = 0; i < input.length() && i < 512; i++) {
        c = input.charAt(i);

        // when char is an unescaped
        if (!escaped && (c == BACKSLASH)) {
            escaped = true;
        }

        // (when char is a unescaped space AND it's not within quotes) OR char is \r or \n
        else if (((c == SPACE) && !escaped && !withinQuotes) || (c == CARRIAGERETURN) || (c == NEWLINE)) {
            // when tmp string isn't empty, add it to the list
            if (tmp.length() > 0) {
                list->add(tmp);
                tmp = String(); // reset tmp string
            }
        }

        // when char is an unescaped "
        else if ((c == DOUBLEQUOTES) && !escaped) {
            // update wheter or not the following chars are within quotes or not
            withinQuotes = !withinQuotes;

            if ((tmp.length() == 0) && !withinQuotes) tmp += SPACE;  // when exiting quotes and tmp string is empty, add
                                                                     // a space
        }

        // add character to tmp string
        else {
            tmp    += c;
            escaped = false;
        }
    }

    // add string if something is left from the loop above
    if (tmp.length() > 0) list->add(tmp);

    // stop when input is empty/invalid
    if (list->size() == 0) return;

    // print comments
    if (list->get(0) == str(CLI_COMMENT)) {
        prntln(input);
        return;
    }

    // if (settings::getCLISettings().serial_echo) {
    //     // print command
        prnt(CLI_INPUT_PREFIX);
        prntln(input);
    // }

    if (list->size() == 0) return;

    // ===== HELP ===== //
    if (eqlsCMD(0, CLI_HELP)) {
        prntln(CLI_HELP_HEADER);

        prntln(CLI_HELP_HELP);
    }
    // ===== NOT FOUND ===== //
    else {
        prnt(CLI_ERROR_NOT_FOUND_A);
        prnt(input);
        prntln(CLI_ERROR_NOT_FOUND_B);
        // some debug stuff

        /*
           Serial.println(list->get(0));
           for(int i=0;i<input.length();i++){
           Serial.print(input.charAt(i), HEX);
           Serial.print(' ');
           }
         */
    }
}
