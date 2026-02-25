const MIDI_SYS_SET_NAME = 0;
const MIDI_SYS_SET_VALUE = 1;
const MIDI_SYS_SET_CHANNEL = 2;
const MIDI_SYS_SET_CC = 3;
const MIDI_SYS_SET_RANGE_MIN = 4;
const MIDI_SYS_SET_RANGE_MAX = 5;
const MIDI_SYS_SET_COLOR_BG = 6;
const MIDI_SYS_SET_COLOR_BORDER = 7;
const MIDI_SYS_SET_COLOR_TEXT = 8;
const MIDI_SYS_SET_COLOR_BAR = 9;
const MIDI_SYS_FIRMWARE_UPDATE = 10;
const MIDI_SYS_GET_NAME = 11;
const MIDI_SYS_GET_VALUE = 12;
const MIDI_SYS_GET_CHANNEL = 13;
const MIDI_SYS_GET_CC = 14;
const MIDI_SYS_GET_RANGE_MIN = 15;
const MIDI_SYS_GET_RANGE_MAX = 16;
const MIDI_SYS_GET_COLOR_BG = 17;
const MIDI_SYS_GET_COLOR_BORDER = 18;
const MIDI_SYS_GET_COLOR_TEXT = 19;
const MIDI_SYS_GET_COLOR_BAR = 20;
const MIDI_SYS_SET_STEP = 21;
const MIDI_SYS_SET_BUTTON_ONCLICK_MODE = 22;
const MIDI_SYS_SET_BUTTON_ONCLICK_STEP = 23;
const MIDI_SYS_SET_BUTTON_MIDI_CHANNEL = 24;
const MIDI_SYS_SET_BUTTON_MIDI_CC = 25;
const MIDI_SYS_SET_BUTTON_MIDI_RELEASED_VALUE = 26;
const MIDI_SYS_SET_BUTTON_MIDI_PRESSED_VALUE = 27;
const MIDI_SYS_GET_STEP = 28;
const MIDI_SYS_GET_BUTTON_ONCLICK_MODE = 29;
const MIDI_SYS_GET_BUTTON_ONCLICK_STEP = 30;
const MIDI_SYS_GET_BUTTON_MIDI_CHANNEL = 31;
const MIDI_SYS_GET_BUTTON_MIDI_CC = 32;
const MIDI_SYS_GET_BUTTON_MIDI_RELEASED_VALUE = 33;
const MIDI_SYS_GET_BUTTON_MIDI_PRESSED_VALUE = 34;
const MIDI_SYS_SET_SIMPLE_SCREEN = 35;
const MIDI_SYS_GET_SIMPLE_SCREEN = 36;
const MIDI_SYS_SET_BUTTON_ONCLICK_ACTIVE = 37;
const MIDI_SYS_GET_BUTTON_ONCLICK_ACTIVE = 38;

const BUTTON_ONCLICK_STEP = 0;
const BUTTON_MIDI_ENABLED = 0;

const TARGET_DEVICE_NAME = "Hexadeck Controller";
const DFU_TARGET_MEMORY = "@Internal Flash /0x08000000/04*016Kg,01*064Kg,03*128Kg";
const DFU_START_ADDRESS = 0x08000000;
const DFU_TRANSFER_SIZE_FALLBACK = 2048;

const state = {
  midiAccess: null,
  output: null,
  input: null,
  selectedId: null,
  selectedTab: "display",
  connected: false,
  demoMode: false,
  modules: [],
  lastSyncAt: 0,
  logCache: {},
  lastInputId: null,
  lastOutputId: null,
};

const SYNC_COOLDOWN_MS = 1200;

const grid = document.getElementById("grid");
const statusEl = document.getElementById("status");
const outputSelect = document.getElementById("outputSelect");
const inputSelect = document.getElementById("inputSelect");
const connectButton = document.getElementById("connectButton");
const demoButton = document.getElementById("demoButton");
const fwButton = document.getElementById("fwButton");
const syncButton = document.getElementById("syncButton");
const panelTitle = document.getElementById("panelTitle");
const panelSubtitle = document.getElementById("panelSubtitle");
const closePanel = document.getElementById("closePanel");
const modulePanel = document.getElementById("panel");
const tabDisplay = document.getElementById("tabDisplay");
const tabKnob = document.getElementById("tabKnob");
const tabButton = document.getElementById("tabButton");
const displayForm = document.getElementById("displayForm");
const knobForm = document.getElementById("knobForm");
const buttonForm = document.getElementById("buttonForm");
const lockOverlay = document.getElementById("lockOverlay");
const dfuFileInput = document.getElementById("dfuFileInput");
const dfuProgress = document.getElementById("dfuProgress");
const dfuProgressBar = document.getElementById("dfuProgressBar");
const dfuProgressValue = document.getElementById("dfuProgressValue");
const dfuProgressStatus = document.getElementById("dfuProgressStatus");

const nameInput = document.getElementById("nameInput");
const bgColor = document.getElementById("bgColor");
const borderColor = document.getElementById("borderColor");
const textColor = document.getElementById("textColor");
const barColor = document.getElementById("barColor");
const simpleModeToggle = document.getElementById("simpleModeToggle");

const knobChannelInput = document.getElementById("knobChannelInput");
const knobCCInput = document.getElementById("knobCCInput");
const knobValueInput = document.getElementById("knobValueInput");
const knobRangeMinInput = document.getElementById("knobRangeMinInput");
const knobRangeMaxInput = document.getElementById("knobRangeMaxInput");
const knobStepInput = document.getElementById("knobStepInput");

const buttonChannelInput = document.getElementById("buttonChannelInput");
const buttonCCInput = document.getElementById("buttonCCInput");
const buttonPressedInput = document.getElementById("buttonPressedInput");
const buttonReleasedInput = document.getElementById("buttonReleasedInput");
const buttonStepInput = document.getElementById("buttonStepInput");

const moduleEls = [];

function setStatus(text, ready) {
  statusEl.textContent = text;
  statusEl.classList.toggle("ready", ready);
  statusEl.classList.toggle("idle", !ready);
}

function setDfuStatus(text, isError = false) {
  if (!dfuProgressStatus) {
    return;
  }
  dfuProgressStatus.textContent = text;
  dfuProgressStatus.classList.toggle("error", isError);
}

function setDfuProgressState(visible, value, total) {
  if (!dfuProgress || !dfuProgressBar || !dfuProgressValue) {
    return;
  }
  dfuProgress.classList.toggle("hidden", !visible);
  if (!visible) {
    return;
  }
  if (typeof total === "number" && Number.isFinite(total)) {
    dfuProgressBar.max = total;
  }
  if (typeof value === "number" && Number.isFinite(value)) {
    dfuProgressBar.value = value;
  }
  if (typeof total === "number" && total > 0) {
    const percent = Math.min(100, Math.round((dfuProgressBar.value / total) * 100));
    dfuProgressValue.textContent = `${percent}%`;
  } else {
    dfuProgressValue.textContent = "";
  }
}

function setConnected(connected, statusText) {
  state.connected = connected;
  document.body.classList.toggle("connected", connected);
  connectButton.classList.toggle("connected", connected);
  connectButton.textContent = connected ? "Connected" : "Connect MIDI";
  connectButton.disabled = state.demoMode;
  if (statusText) {
    setStatus(statusText, connected);
  }
  if (!connected) {
    state.selectedId = null;
    panelTitle.textContent = "Select a module";
    panelSubtitle.textContent = "Click a display or encoder to edit.";
    [...document.querySelectorAll(".module")].forEach((el) => el.classList.remove("selected"));
  }
  updatePanelSelectionState();
  if (lockOverlay) {
    lockOverlay.setAttribute("aria-hidden", connected ? "true" : "false");
  }
}

function updatePanelSelectionState() {
  if (!modulePanel) {
    return;
  }
  const shouldDisable = !state.demoMode && state.selectedId === null;
  modulePanel.classList.toggle("panel-disabled", shouldDisable);
}

function isTargetOutput(output) {
  if (!output) {
    return false;
  }
  const name = (output.name || "").toLowerCase();
  return name.includes(TARGET_DEVICE_NAME.toLowerCase());
}

function createDefaultModuleState(id) {
  return {
    id,
    name: `Name${id}`,
    value: 64,
    min: 0,
    max: 127,
    step: 1,
    channel: 0,
    cc: 0,
    midiMapReady: {
      channel: false,
      cc: false,
    },
    simple: true,
    colors: {
      bg: bgColor ? bgColor.value : "#1e1e1e",
      border: borderColor ? borderColor.value : "#ffffff",
      text: textColor ? textColor.value : "#ffffff",
      bar: barColor ? barColor.value : "#00ff88",
    },
    button: {
      channel: 0,
      cc: 0,
      pressed: 127,
      released: 0,
      step: 1,
      midiEnabled: true,
      midiMapReady: {
        channel: false,
        cc: false,
      },
      isPressed: false,
      onclickMode: 0,
      onclickActive: 0,
    },
  };
}

function buildGrid() {
  if (!state.modules.length) {
    for (let i = 0; i < 16; i += 1) {
      state.modules.push(createDefaultModuleState(i));
    }
  }

  for (let i = 0; i < 16; i += 1) {
    const moduleEl = document.createElement("div");
    moduleEl.className = "module";
    moduleEl.dataset.moduleId = String(i);

    const screen = document.createElement("div");
    screen.className = "screen";
    screen.dataset.type = "display";

    const screenHeader = document.createElement("div");
    screenHeader.className = "screen-header";

    const screenChannel = document.createElement("span");
    screenChannel.className = "screen-channel";
    screenChannel.textContent = "CH-1";

    const screenCC = document.createElement("span");
    screenCC.className = "screen-cc";
    screenCC.textContent = "CC-0";

    const screenStep = document.createElement("span");
    screenStep.className = "screen-step";
    screenStep.textContent = "S:1";

    screenHeader.appendChild(screenChannel);
    screenHeader.appendChild(screenCC);
    screenHeader.appendChild(screenStep);

    const screenName = document.createElement("div");
    screenName.className = "screen-name";
    screenName.textContent = `Name${i}`;

    const screenValueRow = document.createElement("div");
    screenValueRow.className = "screen-value-row";

    const screenMin = document.createElement("span");
    screenMin.className = "screen-min";
    screenMin.textContent = "0";

    const screenValue = document.createElement("span");
    screenValue.className = "screen-value";
    screenValue.textContent = "64";

    const screenMax = document.createElement("span");
    screenMax.className = "screen-max";
    screenMax.textContent = "127";

    screenValueRow.appendChild(screenMin);
    screenValueRow.appendChild(screenValue);
    screenValueRow.appendChild(screenMax);

    const screenBar = document.createElement("div");
    screenBar.className = "screen-bar";
    const screenBarFill = document.createElement("div");
    screenBarFill.className = "screen-bar-fill";
    screenBar.appendChild(screenBarFill);

    screen.appendChild(screenHeader);
    screen.appendChild(screenName);
    screen.appendChild(screenValueRow);
    screen.appendChild(screenBar);
    screen.dataset.type = "display";

    const knob = document.createElement("div");
    knob.className = "knob";
    knob.dataset.type = "knob";

    moduleEl.appendChild(screen);
    moduleEl.appendChild(knob);
    moduleEl.addEventListener("click", (event) => {
      const target = event.target;
      const hit = target.closest(".screen, .knob");
      const type = hit && hit.classList.contains("knob") ? "knob" : "display";
      selectModule(i, type);
    });

    grid.appendChild(moduleEl);

    moduleEls[i] = {
      moduleEl,
      screen,
      screenChannel,
      screenCC,
      screenStep,
      screenName,
      screenMin,
      screenValue,
      screenMax,
      screenBarFill,
      knob,
    };

    updateModuleUI(i);
  }
}

function updateModuleUI(id) {
  const moduleState = state.modules[id];
  const els = moduleEls[id];
  if (!moduleState || !els) {
    return;
  }

  const channelDisplay = moduleState.channel + 1;
  els.screenChannel.textContent = `CH-${channelDisplay}`;
  els.screenCC.textContent = `CC-${moduleState.cc}`;
  els.screenStep.textContent = `S:${moduleState.step}`;
  els.screenName.textContent = moduleState.name || `Name${id}`;
  els.screenMin.textContent = String(moduleState.min);
  els.screenValue.textContent = String(moduleState.value);
  els.screenMax.textContent = String(moduleState.max);

  const range = Math.max(1, moduleState.max - moduleState.min);
  const normalized = Math.min(Math.max((moduleState.value - moduleState.min) / range, 0), 1);
  els.screenBarFill.style.width = `${Math.round(normalized * 100)}%`;

  els.screen.classList.toggle("simple", moduleState.simple);
  els.screen.style.setProperty("--screen-bg", moduleState.colors.bg);
  const borderColor = moduleState.button.onclickActive ? "#00ff88" : "#ffffff";
  els.screen.style.setProperty("--screen-border", borderColor);
  els.screen.style.setProperty("--screen-text", moduleState.colors.text);
  els.screen.style.setProperty("--screen-bar", moduleState.colors.bar);
  const knobBorder = moduleState.button.isPressed ? "#00ff88" : "#f7f3ea";
  els.knob.style.setProperty("--knob-border", knobBorder);
  const knobGlow = moduleState.button.isPressed ? "0 0 10px rgba(0, 255, 136, 0.55)" : "none";
  els.knob.style.setProperty("--knob-glow", knobGlow);
}

function applyModuleToForms(id) {
  const moduleState = state.modules[id];
  if (!moduleState) {
    return;
  }
  nameInput.value = moduleState.name;
  bgColor.value = moduleState.colors.bg;
  borderColor.value = moduleState.colors.border;
  textColor.value = moduleState.colors.text;
  barColor.value = moduleState.colors.bar;
  simpleModeToggle.checked = moduleState.simple;

  knobChannelInput.value = moduleState.channel;
  knobCCInput.value = moduleState.cc;
  knobValueInput.value = moduleState.value;
  knobRangeMinInput.value = moduleState.min;
  knobRangeMaxInput.value = moduleState.max;
  knobStepInput.value = moduleState.step;

  buttonChannelInput.value = moduleState.button.channel;
  buttonCCInput.value = moduleState.button.cc;
  buttonPressedInput.value = moduleState.button.pressed;
  buttonReleasedInput.value = moduleState.button.released;
  buttonStepInput.value = moduleState.button.step;
}

function selectModule(id, type) {
  state.selectedId = id;
  state.selectedTab = type === "knob" ? "knob" : "display";
  [...document.querySelectorAll(".module")].forEach((el) => {
    el.classList.toggle("selected", Number(el.dataset.moduleId) === id);
  });
  panelTitle.textContent = `Module ${id}`;
  panelSubtitle.textContent = type === "knob" ? "Knob settings" : "Display settings";
  applyModuleToForms(id);
  setTab(state.selectedTab);
  updatePanelSelectionState();
}

function setTab(tab) {
  state.selectedTab = tab;
  tabDisplay.classList.toggle("active", tab === "display");
  tabKnob.classList.toggle("active", tab === "knob");
  tabButton.classList.toggle("active", tab === "button");
  displayForm.classList.toggle("hidden", tab !== "display");
  knobForm.classList.toggle("hidden", tab !== "knob");
  buttonForm.classList.toggle("hidden", tab !== "button");
  if (tab === "button") {
    panelSubtitle.textContent = "Button settings";
  }
}

function ensureOutput(options = {}) {
  const { requireSelection = true } = options;
  if (state.demoMode) {
    setStatus("Demo mode: MIDI disabled", false);
    return false;
  }
  if (!state.output) {
    setStatus("Select a MIDI output", false);
    return false;
  }
  if (!state.connected) {
    setStatus("Connect MIDI first", false);
    return false;
  }
  if (requireSelection && state.selectedId === null) {
    setStatus("Select a module first", false);
    return false;
  }
  return true;
}

function sendSysEx(dataBytes, options) {
  if (!ensureOutput(options)) {
    return;
  }
  const message = [0xf0, ...dataBytes, 0xf7];
  state.output.send(message);
}

function encodeColor(hex) {
  const clean = hex.replace("#", "");
  const r = parseInt(clean.slice(0, 2), 16);
  const g = parseInt(clean.slice(2, 4), 16);
  const b = parseInt(clean.slice(4, 6), 16);
  return [r & 0x7f, r >> 7, g & 0x7f, g >> 7, b & 0x7f, b >> 7];
}

function sendName() {
  const name = nameInput.value.trim().slice(0, 16);
  const bytes = Array.from(name).map((c) => c.charCodeAt(0));
  bytes.push(0x00);
  sendSysEx([MIDI_SYS_SET_NAME, state.selectedId, ...bytes]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].name = name;
    updateModuleUI(state.selectedId);
  }
}

function sendValue(value) {
  const clamped = clamp(value, 0, 127);
  sendSysEx([MIDI_SYS_SET_VALUE, state.selectedId, clamped]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].value = clamped;
    updateModuleUI(state.selectedId);
  }
}

function sendChannel(value) {
  const clamped = clamp(value, 0, 15);
  sendSysEx([MIDI_SYS_SET_CHANNEL, state.selectedId, clamped]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].channel = clamped;
    state.modules[state.selectedId].midiMapReady.channel = true;
    updateModuleUI(state.selectedId);
  }
}

function sendCC(value) {
  const clamped = clamp(value, 0, 127);
  sendSysEx([MIDI_SYS_SET_CC, state.selectedId, clamped]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].cc = clamped;
    state.modules[state.selectedId].midiMapReady.cc = true;
    updateModuleUI(state.selectedId);
  }
}

function sendRangeMin(value) {
  const clamped = clamp(value, 0, 127);
  sendSysEx([MIDI_SYS_SET_RANGE_MIN, state.selectedId, clamped]);
  if (state.selectedId !== null) {
    const moduleState = state.modules[state.selectedId];
    moduleState.min = Math.min(clamped, moduleState.max);
    if (moduleState.value < moduleState.min) {
      moduleState.value = moduleState.min;
    }
    updateModuleUI(state.selectedId);
  }
}

function sendRangeMax(value) {
  const clamped = clamp(value, 0, 127);
  sendSysEx([MIDI_SYS_SET_RANGE_MAX, state.selectedId, clamped]);
  if (state.selectedId !== null) {
    const moduleState = state.modules[state.selectedId];
    moduleState.max = Math.max(clamped, moduleState.min);
    if (moduleState.value > moduleState.max) {
      moduleState.value = moduleState.max;
    }
    updateModuleUI(state.selectedId);
  }
}

function sendStep(value) {
  const clamped = clamp(value, 1, 127);
  sendSysEx([MIDI_SYS_SET_STEP, state.selectedId, clamped]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].step = clamped;
    updateModuleUI(state.selectedId);
  }
}

function sendColor(eventType, hex) {
  sendSysEx([eventType, state.selectedId, ...encodeColor(hex)]);
  if (state.selectedId !== null) {
    const moduleState = state.modules[state.selectedId];
    if (eventType === MIDI_SYS_SET_COLOR_BG) {
      moduleState.colors.bg = hex;
    } else if (eventType === MIDI_SYS_SET_COLOR_BORDER) {
      moduleState.colors.border = hex;
    } else if (eventType === MIDI_SYS_SET_COLOR_TEXT) {
      moduleState.colors.text = hex;
    } else if (eventType === MIDI_SYS_SET_COLOR_BAR) {
      moduleState.colors.bar = hex;
    }
    updateModuleUI(state.selectedId);
  }
}

function clamp(value, min, max) {
  const number = Number(value);
  if (Number.isNaN(number)) {
    return min;
  }
  return Math.min(Math.max(number, min), max);
}

function handleMIDIMessage(event) {
  if (state.demoMode) {
    return;
  }
  const data = Array.from(event.data || []);
  if (!data.length) {
    return;
  }

  const status = data[0];
  if (status >= 0xb0 && status <= 0xbf && data.length >= 3) {
    const channel = status & 0x0f;
    const cc = data[1] & 0x7f;
    const value = data[2] & 0x7f;
    state.modules.forEach((moduleState, moduleId) => {
      if (!moduleState) {
        return;
      }
      const knobReady = moduleState.midiMapReady.channel && moduleState.midiMapReady.cc;
      if (knobReady && moduleState.channel === channel && moduleState.cc === cc) {
        const clamped = clamp(value, moduleState.min, moduleState.max);
        if (moduleState.value !== clamped) {
          moduleState.value = clamped;
          applyIncomingState(moduleId);
          logModuleState(moduleId, "midi-cc", { channel, cc, value: clamped });
        }
      }

      const buttonReady = moduleState.button.midiMapReady.channel && moduleState.button.midiMapReady.cc;
      if (!buttonReady || moduleState.button.channel !== channel || moduleState.button.cc !== cc) {
        return;
      }
      if (moduleState.button.onclickMode !== BUTTON_ONCLICK_STEP) {
        return;
      }
      if (moduleState.button.midiEnabled !== true && moduleState.button.midiEnabled !== BUTTON_MIDI_ENABLED) {
        return;
      }
      let shouldUpdate = false;
      if (value === moduleState.button.pressed) {
        if (!moduleState.button.isPressed) {
          moduleState.button.isPressed = true;
          shouldUpdate = true;
          logModuleState(moduleId, "button-press", { channel, cc, value });
        }
      } else if (value === moduleState.button.released) {
        if (moduleState.button.isPressed) {
          moduleState.button.isPressed = false;
          shouldUpdate = true;
          logModuleState(moduleId, "button-release", { channel, cc, value });
        }
        moduleState.button.onclickActive = moduleState.button.onclickActive ? 0 : 1;
        shouldUpdate = true;
        logModuleState(moduleId, "button-onclick", {
          onclickActive: moduleState.button.onclickActive,
          channel,
          cc,
        });
      }
      if (shouldUpdate) {
        applyIncomingState(moduleId);
      }
    });
    return;
  }

  if (data[0] !== 0xf0 || data[data.length - 1] !== 0xf7) {
    return;
  }
  let payload = data.slice(1, -1);
  if (payload[0] === 0x7d) {
    payload = payload.slice(1);
  }
  handleSysexMessage(payload);
}

function decodeColor(bytes) {
  const r = bytes[0] | (bytes[1] << 7);
  const g = bytes[2] | (bytes[3] << 7);
  const b = bytes[4] | (bytes[5] << 7);
  const toHex = (val) => val.toString(16).padStart(2, "0");
  return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
}

function eventName(eventType) {
  switch (eventType) {
    case MIDI_SYS_GET_NAME:
      return "GET_NAME";
    case MIDI_SYS_GET_VALUE:
      return "GET_VALUE";
    case MIDI_SYS_GET_CHANNEL:
      return "GET_CHANNEL";
    case MIDI_SYS_GET_CC:
      return "GET_CC";
    case MIDI_SYS_GET_RANGE_MIN:
      return "GET_RANGE_MIN";
    case MIDI_SYS_GET_RANGE_MAX:
      return "GET_RANGE_MAX";
    case MIDI_SYS_GET_STEP:
      return "GET_STEP";
    case MIDI_SYS_GET_COLOR_BG:
      return "GET_COLOR_BG";
    case MIDI_SYS_GET_COLOR_BORDER:
      return "GET_COLOR_BORDER";
    case MIDI_SYS_GET_COLOR_TEXT:
      return "GET_COLOR_TEXT";
    case MIDI_SYS_GET_COLOR_BAR:
      return "GET_COLOR_BAR";
    case MIDI_SYS_GET_BUTTON_MIDI_CHANNEL:
      return "GET_BUTTON_CHANNEL";
    case MIDI_SYS_GET_BUTTON_MIDI_CC:
      return "GET_BUTTON_CC";
    case MIDI_SYS_GET_BUTTON_MIDI_RELEASED_VALUE:
      return "GET_BUTTON_RELEASED";
    case MIDI_SYS_GET_BUTTON_MIDI_PRESSED_VALUE:
      return "GET_BUTTON_PRESSED";
    case MIDI_SYS_GET_BUTTON_ONCLICK_STEP:
      return "GET_BUTTON_STEP";
    case MIDI_SYS_GET_BUTTON_ONCLICK_MODE:
      return "GET_BUTTON_MODE";
    case MIDI_SYS_GET_BUTTON_ONCLICK_ACTIVE:
      return "GET_BUTTON_ACTIVE";
    case MIDI_SYS_GET_SIMPLE_SCREEN:
      return "GET_SIMPLE_SCREEN";
    default:
      return `EVENT_${eventType}`;
  }
}

function logModuleState(moduleId, label, data) {
  const moduleState = state.modules[moduleId];
  const moduleName = moduleState && moduleState.name ? moduleState.name : `Module ${moduleId}`;
  const cacheKey = `${moduleId}:${label}`;
  const serialized = JSON.stringify(data);
  if (state.logCache[cacheKey] === serialized) {
    return;
  }
  state.logCache[cacheKey] = serialized;
  console.info(`[MIDI] Module ${moduleId} (${moduleName}) ${label}:`, data);
}

function requestAllState(reason = "unknown") {
  state.logCache = {};
  const now = Date.now();
  if (now - state.lastSyncAt < SYNC_COOLDOWN_MS) {
    console.info(`[MIDI] Sync skipped (${reason})`);
    return;
  }
  state.lastSyncAt = now;
  console.info(`[MIDI] Sync start (${reason}): requesting device state...`);
  const perModuleEvents = [
    MIDI_SYS_GET_NAME,
    MIDI_SYS_GET_VALUE,
    MIDI_SYS_GET_CHANNEL,
    MIDI_SYS_GET_CC,
    MIDI_SYS_GET_RANGE_MIN,
    MIDI_SYS_GET_RANGE_MAX,
    MIDI_SYS_GET_STEP,
    MIDI_SYS_GET_COLOR_BG,
    MIDI_SYS_GET_COLOR_BORDER,
    MIDI_SYS_GET_COLOR_TEXT,
    MIDI_SYS_GET_COLOR_BAR,
    MIDI_SYS_GET_BUTTON_MIDI_CHANNEL,
    MIDI_SYS_GET_BUTTON_MIDI_CC,
    MIDI_SYS_GET_BUTTON_MIDI_RELEASED_VALUE,
    MIDI_SYS_GET_BUTTON_MIDI_PRESSED_VALUE,
    MIDI_SYS_GET_BUTTON_ONCLICK_STEP,
    MIDI_SYS_GET_BUTTON_ONCLICK_MODE,
    MIDI_SYS_GET_BUTTON_ONCLICK_ACTIVE,
    MIDI_SYS_GET_SIMPLE_SCREEN,
  ];

  const modules = 16;
  const eventsPerModule = perModuleEvents.length;
  const baseDelay = 1;
  const eventGap = 3;
  const moduleGap = 1;
  let offset = 0;

  for (let id = 0; id < modules; id += 1) {
    for (let e = 0; e < eventsPerModule; e += 1) {
      const eventType = perModuleEvents[e];
      setTimeout(() => {
        console.debug(`[MIDI] GET ${eventName(eventType)} module=${id}`);
        sendSysEx([eventType, id], { requireSelection: false });
      }, offset + baseDelay + e * eventGap);
    }
    offset += eventsPerModule * eventGap + moduleGap;
  }
}

function applyIncomingState(moduleId) {
  updateModuleUI(moduleId);
  if (state.selectedId === moduleId) {
    applyModuleToForms(moduleId);
  }
}

function handleSysexMessage(payload) {
  if (payload.length < 2) {
    return;
  }
  const eventType = payload[0];
  const moduleId = payload[1];
  if (moduleId > 15) {
    return;
  }
  const moduleState = state.modules[moduleId];
  if (!moduleState) {
    return;
  }

  switch (eventType) {
    case MIDI_SYS_GET_NAME: {
      const nameBytes = payload.slice(2);
      const nullIndex = nameBytes.indexOf(0x00);
      const trimmed = nullIndex === -1 ? nameBytes : nameBytes.slice(0, nullIndex);
      moduleState.name = String.fromCharCode(...trimmed);
      applyIncomingState(moduleId);
      logModuleState(moduleId, "name", { name: moduleState.name });
      break;
    }
    case MIDI_SYS_GET_VALUE:
      moduleState.value = payload[2] ?? moduleState.value;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "value", { value: moduleState.value });
      break;
    case MIDI_SYS_GET_CHANNEL:
      moduleState.channel = payload[2] ?? moduleState.channel;
      moduleState.midiMapReady.channel = true;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "channel", { channel: moduleState.channel });
      break;
    case MIDI_SYS_GET_CC:
      moduleState.cc = payload[2] ?? moduleState.cc;
      moduleState.midiMapReady.cc = true;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "cc", { cc: moduleState.cc });
      break;
    case MIDI_SYS_GET_RANGE_MIN:
      moduleState.min = payload[2] ?? moduleState.min;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "range-min", { min: moduleState.min });
      break;
    case MIDI_SYS_GET_RANGE_MAX:
      moduleState.max = payload[2] ?? moduleState.max;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "range-max", { max: moduleState.max });
      break;
    case MIDI_SYS_GET_STEP:
      moduleState.step = payload[2] ?? moduleState.step;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "step", { step: moduleState.step });
      break;
    case MIDI_SYS_GET_SIMPLE_SCREEN:
      moduleState.simple = (payload[2] ?? 0) !== 0;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "simple", { simple: moduleState.simple });
      break;
    case MIDI_SYS_GET_COLOR_BG:
      moduleState.colors.bg = decodeColor(payload.slice(2, 8));
      applyIncomingState(moduleId);
      logModuleState(moduleId, "color-bg", { bg: moduleState.colors.bg });
      break;
    case MIDI_SYS_GET_COLOR_BORDER:
      moduleState.colors.border = decodeColor(payload.slice(2, 8));
      applyIncomingState(moduleId);
      logModuleState(moduleId, "color-border", { border: moduleState.colors.border });
      break;
    case MIDI_SYS_GET_COLOR_TEXT:
      moduleState.colors.text = decodeColor(payload.slice(2, 8));
      applyIncomingState(moduleId);
      logModuleState(moduleId, "color-text", { text: moduleState.colors.text });
      break;
    case MIDI_SYS_GET_COLOR_BAR:
      moduleState.colors.bar = decodeColor(payload.slice(2, 8));
      applyIncomingState(moduleId);
      logModuleState(moduleId, "color-bar", { bar: moduleState.colors.bar });
      break;
    case MIDI_SYS_GET_BUTTON_MIDI_CHANNEL:
      moduleState.button.channel = payload[2] ?? moduleState.button.channel;
      moduleState.button.midiMapReady.channel = true;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "button-channel", { buttonChannel: moduleState.button.channel });
      break;
    case MIDI_SYS_GET_BUTTON_MIDI_CC:
      moduleState.button.cc = payload[2] ?? moduleState.button.cc;
      moduleState.button.midiMapReady.cc = true;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "button-cc", { buttonCC: moduleState.button.cc });
      break;
    case MIDI_SYS_GET_BUTTON_MIDI_RELEASED_VALUE:
      moduleState.button.released = payload[2] ?? moduleState.button.released;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "button-released", { buttonReleased: moduleState.button.released });
      break;
    case MIDI_SYS_GET_BUTTON_MIDI_PRESSED_VALUE:
      moduleState.button.pressed = payload[2] ?? moduleState.button.pressed;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "button-pressed", { buttonPressed: moduleState.button.pressed });
      break;
    case MIDI_SYS_GET_BUTTON_ONCLICK_STEP:
      moduleState.button.step = payload[2] ?? moduleState.button.step;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "button-step", { buttonStep: moduleState.button.step });
      break;
    case MIDI_SYS_GET_BUTTON_ONCLICK_MODE:
      moduleState.button.onclickMode = payload[2] ?? moduleState.button.onclickMode;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "button-mode", { onclickMode: moduleState.button.onclickMode });
      break;
    case MIDI_SYS_GET_BUTTON_ONCLICK_ACTIVE:
      moduleState.button.onclickActive = payload[2] ?? moduleState.button.onclickActive;
      applyIncomingState(moduleId);
      logModuleState(moduleId, "button-active", { onclickActive: moduleState.button.onclickActive });
      break;
    default:
      break;
  }
}

function chooseDfuInterface(interfaces) {
  if (!interfaces || !interfaces.length) {
    return null;
  }
  const exact = interfaces.find((intf) => {
    const name = intf.name || "";
    return (
      intf.alternate.interfaceProtocol === 0x02 &&
      (name.includes(DFU_TARGET_MEMORY) || (name.includes("@Internal Flash") && name.includes("0x08000000")))
    );
  });
  if (exact) {
    return exact;
  }
  return interfaces.find((intf) => intf.alternate.interfaceProtocol === 0x02) || interfaces[0];
}

async function fixInterfaceNames(device, interfaces) {
  if (!interfaces.some((intf) => intf.name == null)) {
    return;
  }
  const tempDevice = new dfu.Device(device, interfaces[0]);
  await tempDevice.device_.open();
  await tempDevice.device_.selectConfiguration(1);
  const mapping = await tempDevice.readInterfaceNames();
  await tempDevice.close();

  interfaces.forEach((intf) => {
    if (intf.name !== null) {
      return;
    }
    const configIndex = intf.configuration.configurationValue;
    const intfNumber = intf["interface"].interfaceNumber;
    const alt = intf.alternate.alternateSetting;
    if (mapping?.[configIndex]?.[intfNumber]?.[alt]) {
      intf.name = mapping[configIndex][intfNumber][alt];
    }
  });
}

async function getDFUDescriptorProperties(device) {
  try {
    const data = await device.readConfigurationDescriptor(0);
    const configDesc = dfu.parseConfigurationDescriptor(data);
    const configValue = device.settings.configuration.configurationValue;
    let funcDesc = null;
    if (configDesc.bConfigurationValue === configValue) {
      for (const desc of configDesc.descriptors) {
        if (desc.bDescriptorType === 0x21 && Object.prototype.hasOwnProperty.call(desc, "bcdDFUVersion")) {
          funcDesc = desc;
          break;
        }
      }
    }
    if (!funcDesc) {
      return {};
    }
    return {
      WillDetach: (funcDesc.bmAttributes & 0x08) !== 0,
      ManifestationTolerant: (funcDesc.bmAttributes & 0x04) !== 0,
      CanUpload: (funcDesc.bmAttributes & 0x02) !== 0,
      CanDnload: (funcDesc.bmAttributes & 0x01) !== 0,
      TransferSize: funcDesc.wTransferSize,
      DetachTimeOut: funcDesc.wDetachTimeOut,
      DFUVersion: funcDesc.bcdDFUVersion,
    };
  } catch (error) {
    return {};
  }
}

async function startDfuUpdate(file) {
  if (!navigator.usb) {
    setDfuStatus("WebUSB not available in this browser.", true);
    return;
  }

  if (state.output && state.connected) {
    state.output.send([0xf0, MIDI_SYS_FIRMWARE_UPDATE, 0xf7]);
    setDfuStatus("Requested bootloader. Select the DFU device...");
  }

  let selectedDevice;
  try {
    selectedDevice = await navigator.usb.requestDevice({
      filters: [{ classCode: 0xfe, subclassCode: 0x01 }],
    });
  } catch (error) {
    setDfuStatus("DFU device selection cancelled.");
    return;
  }

  let interfaces = dfu.findDeviceDfuInterfaces(selectedDevice);
  if (!interfaces.length) {
    throw new Error("No DFU interfaces found on the selected device.");
  }
  await fixInterfaceNames(selectedDevice, interfaces);
  const settings = chooseDfuInterface(interfaces);
  if (!settings) {
    throw new Error("No compatible DFU interface found.");
  }
  if (settings.alternate.interfaceProtocol !== 0x02) {
    throw new Error("Selected interface is not in DFU mode. Put the device in bootloader mode.");
  }

  const isDfuSe = Boolean(settings.name && settings.name.startsWith("@"));
  const dfuDevice = isDfuSe
    ? new dfuse.Device(selectedDevice, settings)
    : new dfu.Device(selectedDevice, settings);

  dfuDevice.logProgress = (done, total) => {
    if (typeof total !== "undefined") {
      setDfuProgressState(true, done, total);
    } else {
      setDfuProgressState(true, done, file.size);
    }
  };
  dfuDevice.logInfo = (msg) => setDfuStatus(msg);
  dfuDevice.logWarning = (msg) => setDfuStatus(msg);
  dfuDevice.logError = (msg) => setDfuStatus(msg, true);

  await dfuDevice.open();

  const desc = await getDFUDescriptorProperties(dfuDevice);
  const transferSize = desc.TransferSize || DFU_TRANSFER_SIZE_FALLBACK;
  const manifestationTolerant = desc.ManifestationTolerant !== false;

  if (dfuDevice instanceof dfuse.Device) {
    dfuDevice.startAddress = DFU_START_ADDRESS;
  }

  try {
    const status = await dfuDevice.getStatus();
    if (status.state === dfu.dfuERROR) {
      await dfuDevice.clearStatus();
    }
  } catch (error) {
    setDfuStatus("Warning: failed to clear DFU status.");
  }

  const payload = await file.arrayBuffer();
  await dfuDevice.do_download(transferSize, payload, manifestationTolerant);
  setDfuProgressState(true, file.size, file.size);
  setDfuStatus("Update complete. Device resetting...");

  try {
    await dfuDevice.close();
  } catch (error) {
    // Ignore close errors after reset.
  }
}

async function connectMIDI() {
  if (state.demoMode) {
    setStatus("Disable demo mode to connect MIDI", false);
    return;
  }
  if (!navigator.requestMIDIAccess) {
    setStatus("WebMIDI not supported", false);
    return;
  }

  try {
    setConnected(false, "Scanning MIDI devices...");
    console.info("[MIDI] Requesting access...");
    state.midiAccess = await navigator.requestMIDIAccess({ sysex: true });
    console.info("[MIDI] Access granted.");
    populateOutputs();
    populateInputs();
    state.midiAccess.onstatechange = () => {
      console.info("[MIDI] Device state change detected.");
      populateOutputs();
      populateInputs();
    };
  } catch (error) {
    setStatus("SysEx permission denied", false);
    console.error("[MIDI] Access denied.", error);
  }
}

function disconnectMIDI() {
  state.output = null;
  if (state.input) {
    state.input.onmidimessage = null;
  }
  state.input = null;
  outputSelect.innerHTML = "";
  inputSelect.innerHTML = "";
  const option = document.createElement("option");
  option.value = "";
  option.textContent = "Connect to scan devices";
  option.disabled = true;
  option.selected = true;
  outputSelect.appendChild(option);
  const inputOption = option.cloneNode(true);
  inputSelect.appendChild(inputOption);
  outputSelect.disabled = true;
  inputSelect.disabled = true;
  setConnected(false, "Disconnected");
}

function setDemoMode(enabled) {
  state.demoMode = enabled;
  demoButton.textContent = enabled ? "Exit Demo" : "Run Demo";
  demoButton.classList.toggle("connected", enabled);
  syncButton.disabled = enabled;
  fwButton.disabled = enabled;
  outputSelect.disabled = true;
  inputSelect.disabled = true;
  if (enabled) {
    if (state.input) {
      state.input.onmidimessage = null;
    }
    state.input = null;
    state.output = null;
    setConnected(true, "Demo mode (no MIDI)");
  } else {
    setConnected(false, "Disconnected");
  }
}

function populateOutputs() {
  outputSelect.innerHTML = "";
  const outputs = [...state.midiAccess.outputs.values()].filter((output) => isTargetOutput(output));
  console.info("[MIDI] Outputs:", outputs.map((output) => output.name || output.id));
  outputs.forEach((output) => {
    const option = document.createElement("option");
    option.value = output.id;
    option.textContent = output.name || output.id;
    outputSelect.appendChild(option);
  });
  state.output = outputs[0] || null;
  if (state.output) {
    const outputChanged = state.output.id !== state.lastOutputId;
    state.lastOutputId = state.output.id;
    outputSelect.disabled = false;
    outputSelect.value = state.output.id;
    setConnected(true, `Connected to ${state.output.name || TARGET_DEVICE_NAME}`);
    outputSelect.focus();
    console.info("[MIDI] Output selected:", state.output.name || state.output.id);
    if (outputChanged && state.input) {
      requestAllState("output-ready");
    }
  } else {
    const option = document.createElement("option");
    option.value = "";
    option.textContent = "No Hexadeck Controller found";
    option.disabled = true;
    option.selected = true;
    outputSelect.appendChild(option);
    outputSelect.disabled = true;
    setConnected(false, "Hexadeck Controller not found");
  }
}

function populateInputs() {
  inputSelect.innerHTML = "";
  const inputs = [...state.midiAccess.inputs.values()].filter((input) => isTargetOutput(input));
  console.info("[MIDI] Inputs:", inputs.map((input) => input.name || input.id));
  inputs.forEach((input) => {
    const option = document.createElement("option");
    option.value = input.id;
    option.textContent = input.name || input.id;
    inputSelect.appendChild(option);
  });
  if (state.input) {
    state.input.onmidimessage = null;
  }
  state.input = inputs[0] || null;
  if (state.input) {
    const inputChanged = state.input.id !== state.lastInputId;
    state.lastInputId = state.input.id;
    inputSelect.disabled = false;
    inputSelect.value = state.input.id;
    state.input.onmidimessage = handleMIDIMessage;
    console.info("[MIDI] Input selected:", state.input.name || state.input.id);
    if (state.output && (inputChanged || state.lastSyncAt === 0)) {
      requestAllState("input-ready");
    }
  } else {
    const option = document.createElement("option");
    option.value = "";
    option.textContent = "No Hexadeck Controller input";
    option.disabled = true;
    option.selected = true;
    inputSelect.appendChild(option);
    inputSelect.disabled = true;
  }
}

connectButton.addEventListener("click", () => {
  if (state.demoMode) {
    setStatus("Disable demo mode to connect MIDI", false);
    return;
  }
  if (state.connected) {
    disconnectMIDI();
    return;
  }
  connectMIDI();
});
demoButton.addEventListener("click", () => {
  setDemoMode(!state.demoMode);
});
outputSelect.addEventListener("change", (event) => {
  if (!state.midiAccess) {
    return;
  }
  const outputs = [...state.midiAccess.outputs.values()].filter((output) => isTargetOutput(output));
  state.output = outputs.find((out) => out.id === event.target.value) || null;
  if (state.output) {
    setConnected(true, `Connected to ${state.output.name || TARGET_DEVICE_NAME}`);
    requestAllState("output-change");
  }
});

inputSelect.addEventListener("change", (event) => {
  if (!state.midiAccess) {
    return;
  }
  const inputs = [...state.midiAccess.inputs.values()].filter((input) => isTargetOutput(input));
  if (state.input) {
    state.input.onmidimessage = null;
  }
  state.input = inputs.find((input) => input.id === event.target.value) || null;
  if (state.input) {
    state.input.onmidimessage = handleMIDIMessage;
    if (state.output) {
      requestAllState("input-change");
    }
  }
});

fwButton.addEventListener("click", () => {
  if (state.demoMode) {
    setStatus("Demo mode: MIDI disabled", false);
    return;
  }
  if (!dfuFileInput) {
    setDfuStatus("Firmware picker unavailable.", true);
    return;
  }
  dfuFileInput.click();
});

if (dfuFileInput) {
  dfuFileInput.addEventListener("change", async (event) => {
    const file = event.target.files && event.target.files[0];
    if (!file) {
      return;
    }
    setDfuProgressState(true, 0, file.size);
    setDfuStatus(`Selected ${file.name}. Starting DFU...`);
    try {
      await startDfuUpdate(file);
    } catch (error) {
      setDfuStatus(`DFU failed: ${error}`, true);
    } finally {
      dfuFileInput.value = "";
    }
  });
}

closePanel.addEventListener("click", () => {
  state.selectedId = null;
  [...document.querySelectorAll(".module")].forEach((el) => el.classList.remove("selected"));
  panelTitle.textContent = "Select a module";
  panelSubtitle.textContent = "Click a display or encoder to edit.";
  updatePanelSelectionState();
});

tabDisplay.addEventListener("click", () => setTab("display"));
tabKnob.addEventListener("click", () => setTab("knob"));
tabButton.addEventListener("click", () => setTab("button"));

// Display buttons

document.getElementById("sendName").addEventListener("click", sendName);
document.getElementById("sendBgColor").addEventListener("click", () => {
  sendColor(MIDI_SYS_SET_COLOR_BG, bgColor.value);
});
document.getElementById("sendBorderColor").addEventListener("click", () => {
  sendColor(MIDI_SYS_SET_COLOR_BORDER, borderColor.value);
});
document.getElementById("sendTextColor").addEventListener("click", () => {
  sendColor(MIDI_SYS_SET_COLOR_TEXT, textColor.value);
});
document.getElementById("sendBarColor").addEventListener("click", () => {
  sendColor(MIDI_SYS_SET_COLOR_BAR, barColor.value);
});

simpleModeToggle.addEventListener("change", () => {
  if (!ensureOutput()) {
    simpleModeToggle.checked = !simpleModeToggle.checked;
    return;
  }
  const enabled = simpleModeToggle.checked ? 1 : 0;
  sendSysEx([MIDI_SYS_SET_SIMPLE_SCREEN, state.selectedId, enabled]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].simple = enabled !== 0;
    updateModuleUI(state.selectedId);
  }
});

// Knob buttons

document.getElementById("sendKnobChannel").addEventListener("click", () => {
  sendChannel(knobChannelInput.value);
});
document.getElementById("sendKnobCC").addEventListener("click", () => {
  sendCC(knobCCInput.value);
});
document.getElementById("sendKnobValue").addEventListener("click", () => {
  sendValue(knobValueInput.value);
});
document.getElementById("sendKnobRangeMin").addEventListener("click", () => {
  sendRangeMin(knobRangeMinInput.value);
});
document.getElementById("sendKnobRangeMax").addEventListener("click", () => {
  sendRangeMax(knobRangeMaxInput.value);
});
document.getElementById("sendKnobStep").addEventListener("click", () => {
  sendStep(knobStepInput.value);
});

document.getElementById("sendButtonChannel").addEventListener("click", () => {
  const value = clamp(buttonChannelInput.value, 0, 15);
  sendSysEx([MIDI_SYS_SET_BUTTON_MIDI_CHANNEL, state.selectedId, value]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].button.channel = value;
    state.modules[state.selectedId].button.midiMapReady.channel = true;
    applyIncomingState(state.selectedId);
  }
});

document.getElementById("sendButtonCC").addEventListener("click", () => {
  const value = clamp(buttonCCInput.value, 0, 127);
  sendSysEx([MIDI_SYS_SET_BUTTON_MIDI_CC, state.selectedId, value]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].button.cc = value;
    state.modules[state.selectedId].button.midiMapReady.cc = true;
    applyIncomingState(state.selectedId);
  }
});

document.getElementById("sendButtonPressed").addEventListener("click", () => {
  const value = clamp(buttonPressedInput.value, 0, 127);
  sendSysEx([MIDI_SYS_SET_BUTTON_MIDI_PRESSED_VALUE, state.selectedId, value]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].button.pressed = value;
    applyIncomingState(state.selectedId);
  }
});

document.getElementById("sendButtonReleased").addEventListener("click", () => {
  const value = clamp(buttonReleasedInput.value, 0, 127);
  sendSysEx([MIDI_SYS_SET_BUTTON_MIDI_RELEASED_VALUE, state.selectedId, value]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].button.released = value;
    applyIncomingState(state.selectedId);
  }
});

document.getElementById("sendButtonStep").addEventListener("click", () => {
  const value = clamp(buttonStepInput.value, 1, 127);
  sendSysEx([MIDI_SYS_SET_BUTTON_ONCLICK_STEP, state.selectedId, value]);
  if (state.selectedId !== null) {
    state.modules[state.selectedId].button.step = value;
    applyIncomingState(state.selectedId);
  }
});

syncButton.addEventListener("click", () => {
  if (!ensureOutput({ requireSelection: false })) {
    return;
  }
  requestAllState("manual");
});

buildGrid();
setConnected(false, "Disconnected");
updatePanelSelectionState();
