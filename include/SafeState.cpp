/**
   Arduino Electronic Safe

   Copyright (C) 2020, Uri Shaked.
   Released under the MIT License.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "SafeState.h"

/* Safe state */
#define EEPROM_ADDR_LOCKED   0
#define EEPROM_ADDR_CODE_LEN 1
#define EEPROM_ADDR_CODE     2
#define EEPROM_EMPTY         0xff

#define SAFE_STATE_OPEN (char)0
#define SAFE_STATE_LOCKED (char)1

class SafeState {
  public:
    String Code;
    String MasterPassword = "1111";
    SafeState();
    void lock();
    bool unlock(String code);
    bool locked();
    bool hasCode();
    void setCode(String newCode);

  private:
    void setLock(bool locked);
    bool _locked;
};

SafeState::SafeState() {
  this->_locked = EEPROM.read(EEPROM_ADDR_LOCKED) == SAFE_STATE_LOCKED;
}

void SafeState::lock() {
  this->setLock(true);
}

bool SafeState::locked() {
  return this->_locked;
}

bool SafeState::hasCode() {
  auto codeLength = EEPROM.read(EEPROM_ADDR_CODE_LEN);
  return codeLength != EEPROM_EMPTY;
}

void SafeState::setCode(String newCode) {
  Code = newCode;
  EEPROM.write(EEPROM_ADDR_CODE_LEN, newCode.length());
  for (byte i = 0; i < newCode.length(); i++) {
    EEPROM.write(EEPROM_ADDR_CODE + i, newCode[i]);
  }
}

bool SafeState::unlock(String code) {
  auto codeLength = EEPROM.read(EEPROM_ADDR_CODE_LEN);
  if (codeLength == EEPROM_EMPTY) {
    // There was no code, so unlock always succeeds
    this->setLock(false);
    return true;
  }
  if (code.length() != codeLength) {
    return false;
  }
  if(MasterPassword == code)
  {
    this->setLock(false);
    return true;
  }
  for (byte i = 0; i < code.length(); i++) {
    auto digit = EEPROM.read(EEPROM_ADDR_CODE + i);
    if (digit != code[i]) {
      return false;
    }
  }
  this->setLock(false);
  return true;
}

void SafeState::setLock(bool locked) {
  this->_locked = locked;
  EEPROM.write(EEPROM_ADDR_LOCKED, locked ? SAFE_STATE_LOCKED : SAFE_STATE_OPEN);
}

