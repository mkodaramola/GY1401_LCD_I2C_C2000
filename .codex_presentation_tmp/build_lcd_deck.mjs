import fs from "node:fs/promises";
import path from "node:path";
import { Presentation, PresentationFile } from "@oai/artifact-tool";

const OUT = "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/outputs/GY1401_GY0801_LCD_C2000_Brief.pptx";
const TMP = "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/.codex_presentation_tmp";
const GY1401_MAP = "C:/Users/mkoda/Documents/lcd materials/COM_SEG Mapping Table.PNG";
const GY0801_MAP = "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials/gy-modified.PNG";
const GY0801_TRUTH = "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/tmp/pdf_pages/gy0801_truth_table_current.png";

const W = 1280;
const H = 720;
const ink = "#101418";
const muted = "#5F6872";
const light = "#F3F5F7";
const rule = "#C8CDD3";
const accent = "#1E88E5";
const accent2 = "#12A594";

async function imageBytes(file) {
  const bytes = await fs.readFile(file);
  return bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
}

function addText(slide, text, x, y, w, h, opts = {}) {
  const shape = slide.shapes.add({
    geometry: "textbox",
    position: { left: x, top: y, width: w, height: h },
    fill: "none",
    line: { style: "solid", fill: "none", width: 0 },
  });
  shape.text = text;
  shape.text.style = {
    fontSize: opts.size ?? 24,
    bold: opts.bold ?? false,
    color: opts.color ?? ink,
    alignment: opts.align ?? "left",
  };
  return shape;
}

function addRule(slide, x, y, w, color = rule) {
  slide.shapes.add({
    geometry: "rect",
    position: { left: x, top: y, width: w, height: 2 },
    fill: color,
    line: { style: "solid", fill: color, width: 0 },
  });
}

function addPanel(slide, x, y, w, h, fill = light) {
  return slide.shapes.add({
    geometry: "rect",
    position: { left: x, top: y, width: w, height: h },
    fill,
    line: { style: "solid", fill: "#D9DDE2", width: 1 },
  });
}

function addChip(slide, text, x, y, w, color = accent) {
  const chip = slide.shapes.add({
    geometry: "roundRect",
    position: { left: x, top: y, width: w, height: 32 },
    fill: "#FFFFFF",
    line: { style: "solid", fill: color, width: 1.4 },
    borderRadius: 14,
  });
  chip.text = text;
  chip.text.style = { fontSize: 15, bold: true, color };
}

function addBullets(slide, items, x, y, w, lineH = 42, size = 22) {
  items.forEach((item, i) => {
    addText(slide, String(i + 1).padStart(2, "0"), x, y + i * lineH, 44, 28, {
      size: 16,
      bold: true,
      color: accent,
    });
    addText(slide, item, x + 58, y + i * lineH - 2, w - 58, 34, {
      size,
      color: ink,
    });
  });
}

function addTitle(slide, title, kicker) {
  addText(slide, kicker, 56, 40, 500, 30, { size: 15, bold: true, color: muted });
  addText(slide, title, 56, 82, 1030, 88, { size: 42, bold: true, color: ink });
  addRule(slide, 56, 178, 1168);
}

async function addImage(slide, file, x, y, w, h, alt, fit = "contain", crop) {
  slide.images.add({
    blob: await imageBytes(file),
    contentType: "image/png",
    alt,
    fit,
    crop,
    position: { left: x, top: y, width: w, height: h },
  });
}

function addSources(slide, lines) {
  slide.speakerNotes.textFrame.setText([
    "[Sources]",
    ...lines,
  ]);
}

function makeSlide(presentation) {
  const slide = presentation.slides.add();
  slide.background.fill = "#FFFFFF";
  return slide;
}

async function build() {
  await fs.mkdir(path.dirname(OUT), { recursive: true });
  await fs.mkdir(TMP, { recursive: true });

  const p = Presentation.create({ slideSize: { width: W, height: H } });

  {
    const s = makeSlide(p);
    addText(s, "Segmented LCD configuration\nfor C2000 energy meters", 56, 78, 650, 170, {
      size: 50,
      bold: true,
    });
    addText(s, "GY1401 and GY0801 implementation brief", 56, 270, 620, 42, {
      size: 25,
      color: muted,
    });
    addPanel(s, 760, 80, 420, 440, "#F7F9FB");
    await addImage(s, GY0801_MAP, 790, 110, 360, 250, "GY0801 annotated LCD segment map", "contain");
    await addImage(s, GY1401_MAP, 790, 380, 360, 116, "GY1401 COM SEG mapping table", "cover");
    addText(s, "Prepared for future C2000-based energy meter UI integration", 56, 600, 680, 34, {
      size: 20,
      color: muted,
    });
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD",
      "C:/Users/mkoda/Documents/lcd materials",
      "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "GY1401 work established the C2000 LCD path", "GY1401 / C2000 ENERGY METER");
    addText(s, "The implementation proves the CN91C4S48-based GY1401 can be configured and updated from the C2000 firmware using a small reusable LCD abstraction.", 56, 214, 650, 88, {
      size: 24,
      color: muted,
    });
    addBullets(s, [
      "I2C controller path initialized on C2000Ware driverlib.",
      "CN91C4S48 command sequence sets display timing, contrast, blink and enable state.",
      "A display RAM shadow lets numeric and fixed-symbol writes stay isolated.",
    ], 56, 346, 680, 54, 22);
    addPanel(s, 790, 226, 350, 270);
    addText(s, "Core result", 826, 262, 260, 34, { size: 26, bold: true });
    addText(s, "Numbers display correctly through MeterLcd::Print(), which writes packed byte patterns into the LCD main message window.", 826, 320, 260, 130, {
      size: 22,
      color: muted,
    });
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/hw_i2c.cpp",
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/meter_lcd.cpp",
      "C:/Users/mkoda/Documents/lcd materials/CN91C4S48_i2c.pdf",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "Configure GY1401 before segment writes", "GY1401 CONFIGURATION");
    addPanel(s, 56, 224, 1168, 360);
    addBullets(s, [
      "Target address: 0x3E on the I2C bus.",
      "Startup sends ICSET, DISCTL, EVRSET, BLKCTL, APCTL and MODSET.",
      "After enable, DDRAM is cleared so every later update starts from known state.",
      "C2000 I2C transport chunks longer RAM writes and waits for FIFO/STOP completion.",
    ], 92, 270, 610, 54, 23);
    await addImage(s, GY1401_MAP, 750, 245, 420, 280, "GY1401 COM SEG table", "contain");
    addText(s, "Configuration separates controller bring-up from UI content.", 92, 610, 760, 34, {
      size: 20,
      color: muted,
    });
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/includes/meter_lcd.h",
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/meter_lcd.cpp",
      "C:/Users/mkoda/Documents/lcd materials/COM_SEG Mapping Table.PNG",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "Numeric display depends on packed LCD byte patterns", "GY1401 PROGRAMMING");
    addText(s, "The display is not an ASCII LCD. Digits are converted to controller-specific segment bytes, then written at the main message DDRAM window.", 56, 216, 770, 76, {
      size: 24,
      color: muted,
    });
    addPanel(s, 70, 334, 500, 205, "#F7F9FB");
    addText(s, "Example digit LUT", 100, 366, 300, 32, { size: 24, bold: true });
    addText(s, "0 -> 0xFA\n1 -> 0x0A\n8 -> 0xFE\n9 -> 0xCE", 100, 412, 300, 112, {
      size: 25,
      color: ink,
    });
    addPanel(s, 660, 334, 500, 205, "#F7F9FB");
    addText(s, "Write target", 690, 366, 300, 32, { size: 24, bold: true });
    addText(s, "DDRAM 0x18\n11 bytes\nright-aligned digit packing", 690, 412, 360, 84, {
      size: 25,
      color: ink,
    });
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/meter_lcd.cpp",
      "C:/Users/mkoda/Documents/lcd materials/lcd mspm0  code/util_ui_lcd_lu.h",
      "C:/Users/mkoda/Documents/lcd materials/lcd mspm0  code/util_i2c.h",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "Fixed glass symbols are controlled as named segments", "GY1401 SYMBOLS");
    addText(s, "Units and status marks such as kW, kWh, Ah, arrows, warning, signal bars and battery segments are physical glass symbols, not arbitrary text.", 56, 216, 700, 78, {
      size: 24,
      color: muted,
    });
    addBullets(s, [
      "The COM/SEG table maps each etched symbol to a controller RAM bit.",
      "Raw testing uses SetGlassSegment(Kxx, ON/OFF).",
      "Future helpers can group symbols into ShowUnitKWh(), ShowWarning(), and SetBatteryLevel().",
    ], 56, 340, 720, 48, 22);
    await addImage(s, GY1401_MAP, 820, 220, 340, 315, "GY1401 fixed symbol mapping", "cover", { left: 0, top: 0, right: 0.45, bottom: 0.18 });
    addSources(s, [
      "C:/Users/mkoda/Documents/lcd materials/Displayable Characters.txt",
      "C:/Users/mkoda/Documents/lcd materials/COM_SEG Mapping Table.PNG",
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/includes/meter_lcd.h",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "GY1401 is ready to become an energy-meter display layer", "GY1401 APPLICATION PATH");
    addText(s, "The next firmware step is to stop driving demo values directly and bind display content to measured meter states.", 56, 216, 820, 64, {
      size: 24,
      color: muted,
    });
    addBullets(s, [
      "Voltage, current, power, energy and credit values feed numeric fields.",
      "Relay, tariff, warning and communication states feed fixed icons.",
      "The LCD layer remains hardware-specific; the meter UI layer chooses what to show.",
    ], 90, 342, 800, 54, 24);
    addChip(s, "transport", 900, 320, 120);
    addChip(s, "encoding", 900, 374, 120, accent2);
    addChip(s, "state UI", 900, 428, 120, "#8E44AD");
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/app/main.cpp",
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/meter_lcd.cpp",
      "C:/Users/mkoda/Documents/lcd materials/Displayable Characters.txt",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "GY0801 targets a richer status display for PRO CI", "GY0801 / PRO CI TARGET");
    addText(s, "GY0801 uses an HT1621-compatible controller and exposes multiple labeled numeric regions plus application icons for inverter, battery and load states.", 56, 216, 720, 78, {
      size: 24,
      color: muted,
    });
    await addImage(s, GY0801_MAP, 790, 205, 390, 310, "GY0801 annotated LCD artwork", "contain");
    addBullets(s, [
      "Input, battery and output/load values have separate digit groups.",
      "Icons cover AC, PV, battery, output, load, error, overload and flow states.",
      "This makes it a better match for PRO CI operational status screens.",
    ], 56, 350, 700, 48, 22);
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/includes/gy0801_lcd.h",
      "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials/gy-modified.PNG",
      "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials/ht1621b.PDF",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "GY0801 communication is GPIO-driven, not I2C", "GY0801 CONFIGURATION");
    addPanel(s, 56, 224, 1168, 340);
    addBullets(s, [
      "C2000 GPIOs: CS=5, WR=4, DATA=9, backlight=8; RD is unused.",
      "The driver sends HT1621-style command, write and read mode frames.",
      "Startup enables system clock, RC oscillator, 1/3 bias 4-COM mode and LCD output.",
      "A configurable pulse delay keeps timing portable across C2000 clock settings.",
    ], 92, 270, 1000, 52, 23);
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/app/main.cpp",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/gy0801_lcd.cpp",
      "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials/AN0468e HT162x Application Guidelines -HOLTEK (1).pdf",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "GY0801 uses shadow RAM and typed symbols", "GY0801 PROGRAMMING");
    addText(s, "The driver stores a 32-address shadow RAM, updates bits locally, and flushes nibbles to the controller in burst writes.", 56, 216, 720, 76, {
      size: 24,
      color: muted,
    });
    await addImage(s, GY0801_TRUTH, 780, 208, 390, 330, "GY0801 truth table crop", "contain");
    addBullets(s, [
      "writeText() and writeNumber() handle digit rendering.",
      "setIcon() groups several low-level symbols into one display intent.",
      "setSymbol() and setRawSegment() support bring-up and mapping tests.",
    ], 56, 356, 680, 50, 23);
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/gy0801_lcd.cpp",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/includes/gy0801_lcd.h",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/tmp/pdf_pages/gy0801_truth_table_current.png",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "Application helpers already mirror the glass layout", "GY0801 APPLICATION HELPERS");
    addText(s, "The application layer splits visible values into the same regions printed on the glass: INPUT, BATT, OUTPUT and LOAD.", 56, 216, 780, 64, {
      size: 24,
      color: muted,
    });
    addPanel(s, 86, 336, 300, 140, "#F7F9FB");
    addPanel(s, 490, 336, 300, 140, "#F7F9FB");
    addPanel(s, 894, 336, 300, 140, "#F7F9FB");
    addText(s, "writeInput()", 116, 372, 220, 32, { size: 25, bold: true });
    addText(s, "3 digits, DOT1/DOT2", 116, 420, 220, 30, { size: 20, color: muted });
    addText(s, "writeBattery()", 520, 372, 220, 32, { size: 25, bold: true });
    addText(s, "2 middle digits", 520, 420, 220, 30, { size: 20, color: muted });
    addText(s, "writeOutput()", 924, 372, 220, 32, { size: 25, bold: true });
    addText(s, "3 digits, DOT4/DOT5", 924, 420, 240, 30, { size: 20, color: muted });
    addText(s, "These helpers are the natural integration point for PRO CI state formatting.", 86, 560, 820, 34, {
      size: 22,
      color: ink,
    });
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/app/main.cpp",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/includes/gy0801_lcd.h",
    ]);
  }

  {
    const s = makeSlide(p);
    addTitle(s, "Next step: integrate GY0801 into PRO CI state display", "FUTURE PLAN");
    addText(s, "The planned work is to replace the standalone demo loop with a PRO CI display adapter that renders system states consistently.", 56, 216, 820, 70, {
      size: 24,
      color: muted,
    });
    addBullets(s, [
      "Define PRO CI display states: idle, charging, discharging, fault, overload, low battery and communication status.",
      "Map each state to digits, labels, flow arrows, battery gauge and warning icons.",
      "Add a periodic refresh task that updates the GY0801 shadow RAM only when state changes.",
      "Keep hardware driver, display formatting and application state logic in separate modules.",
    ], 76, 326, 1040, 60, 23);
    addText(s, "Outcome: a reusable LCD UI layer that can move from bring-up code into production energy-meter firmware.", 76, 608, 1000, 34, {
      size: 22,
      color: ink,
      bold: true,
    });
    addSources(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD",
      "User-provided future plan: integrate GY0801 in pro_ci to display different states.",
    ]);
  }

  const montage = await p.export({ format: "webp", montage: true, scale: 1 });
  await fs.writeFile(path.join(TMP, "lcd_deck_montage.webp"), Buffer.from(await montage.arrayBuffer()));

  for (const [i, slide] of p.slides.items.entries()) {
    const png = await p.export({ slide, format: "png", scale: 1 });
    await fs.writeFile(path.join(TMP, `slide-${String(i + 1).padStart(2, "0")}.png`), Buffer.from(await png.arrayBuffer()));
    const layout = await slide.export({ format: "layout" });
    await fs.writeFile(path.join(TMP, `slide-${String(i + 1).padStart(2, "0")}.layout.json`), await layout.text());
  }

  const pptx = await PresentationFile.exportPptx(p);
  await pptx.save(OUT);
}

build().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
