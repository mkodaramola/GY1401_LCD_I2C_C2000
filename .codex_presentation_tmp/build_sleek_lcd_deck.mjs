import fs from "node:fs/promises";
import path from "node:path";
import { Presentation, PresentationFile } from "@oai/artifact-tool";

const OUT = "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/outputs/GY1401_GY0801_LCD_C2000_Sleek_Brief.pptx";
const TMP = "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/.codex_presentation_tmp/sleek";
const GY1401_MAP = "C:/Users/mkoda/Documents/lcd materials/COM_SEG Mapping Table.PNG";
const GY0801_MAP = "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials/gy-modified.PNG";
const GY0801_TRUTH = "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/tmp/pdf_pages/gy0801_truth_table_current.png";

const W = 1280;
const H = 720;
const navy = "#071B33";
const blue = "#1E88E5";
const cyan = "#17C3B2";
const amber = "#FFB020";
const green = "#18A058";
const red = "#E5484D";
const ink = "#101418";
const white = "#FFFFFF";
const soft = "#F3F7FB";
const muted = "#52606D";

async function bytes(file) {
  const b = await fs.readFile(file);
  return b.buffer.slice(b.byteOffset, b.byteOffset + b.byteLength);
}

function shape(slide, geometry, x, y, w, h, fill, line = "none", radius = 0) {
  const cfg = {
    geometry,
    position: { left: x, top: y, width: w, height: h },
    fill,
    line: { style: "solid", fill: line, width: line === "none" ? 0 : 1.2 },
  };
  if (radius && (geometry === "rect" || geometry === "textbox" || geometry === "roundRect")) {
    cfg.borderRadius = radius;
  }
  return slide.shapes.add(cfg);
}

function text(slide, value, x, y, w, h, opts = {}) {
  const t = slide.shapes.add({
    geometry: "textbox",
    position: { left: x, top: y, width: w, height: h },
    fill: "none",
    line: { style: "solid", fill: "none", width: 0 },
  });
  t.text = value;
  t.text.style = {
    fontSize: opts.size ?? 28,
    bold: Boolean(opts.bold),
    color: opts.color ?? ink,
    alignment: opts.align ?? "left",
  };
  return t;
}

function label(slide, value, x, y, color = blue) {
  const box = shape(slide, "roundRect", x, y, 138, 34, `${color}`, "none", 16);
  box.text = value;
  box.text.style = { fontSize: 16, bold: true, color: white, alignment: "center" };
}

function title(slide, kicker, headline, theme = blue) {
  shape(slide, "rect", 0, 0, W, 96, navy, "none");
  shape(slide, "rect", 0, 96, W, 8, theme, "none");
  text(slide, kicker, 64, 26, 520, 28, { size: 16, bold: true, color: "#BFD7F2" });
  text(slide, headline, 64, 128, 1040, 100, { size: 46, bold: true, color: ink });
}

function addNotes(slide, sources) {
  slide.speakerNotes.textFrame.setText(["[Sources]", ...sources]);
}

async function addImage(slide, file, x, y, w, h, alt, crop) {
  slide.images.add({
    blob: await bytes(file),
    contentType: "image/png",
    alt,
    fit: "cover",
    crop,
    position: { left: x, top: y, width: w, height: h },
  });
}

function arrow(slide, x, y, w, h, color = blue) {
  shape(slide, "rightArrow", x, y, w, h, color, "none");
}

function node(slide, value, x, y, w, h, fill, stroke = "none", fontSize = 24) {
  const n = shape(slide, "roundRect", x, y, w, h, fill, stroke, 12);
  n.text = value;
  n.text.style = { fontSize, bold: true, color: fill === navy ? white : ink, alignment: "center" };
  return n;
}

function metric(slide, big, small, x, y, color) {
  shape(slide, "roundRect", x, y, 230, 130, white, "#D5E1EF", 18);
  text(slide, big, x + 24, y + 22, 180, 48, { size: 40, bold: true, color });
  text(slide, small, x + 24, y + 74, 176, 42, { size: 21, color: muted });
}

async function build() {
  await fs.mkdir(path.dirname(OUT), { recursive: true });
  await fs.mkdir(TMP, { recursive: true });
  const p = Presentation.create({ slideSize: { width: W, height: H } });

  {
    const s = p.slides.add();
    s.background.fill = navy;
    shape(s, "rect", 760, 0, 520, 720, "#0D2C52", "none");
    shape(s, "rect", 0, 610, 760, 110, blue, "none");
    text(s, "C2000 LCD UI BRING-UP", 64, 66, 420, 28, { size: 18, bold: true, color: "#9DDCFF" });
    text(s, "Segmented LCD drivers for energy meter displays", 64, 126, 650, 220, {
      size: 58,
      bold: true,
      color: white,
    });
    text(s, "GY1401 over I2C + GY0801 over HT1621-style GPIO", 64, 382, 600, 45, {
      size: 28,
      color: "#D9EAFB",
    });
    await addImage(s, GY0801_MAP, 804, 70, 400, 286, "GY0801 annotated LCD map");
    await addImage(s, GY1401_MAP, 804, 390, 400, 205, "GY1401 COM SEG map", { left: 0, top: 0.02, right: 0, bottom: 0.18 });
    text(s, "Future C2000-based energy meter application", 64, 640, 640, 34, { size: 25, bold: true, color: white });
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD",
      "C:/Users/mkoda/Documents/lcd materials",
      "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials",
    ]);
  }

  {
    const s = p.slides.add();
    s.background.fill = soft;
    title(s, "GY1401 CONFIGURATION", "I2C brings the CN91C4S48 glass to life", blue);
    arrow(s, 272, 372, 74, 42, "#9DDCFF");
    arrow(s, 522, 372, 74, 42, "#9DDCFF");
    arrow(s, 772, 372, 74, 42, "#9DDCFF");
    node(s, "C2000\nI2CA", 92, 318, 170, 150, navy);
    node(s, "0x3E\nLCD address", 352, 318, 170, 150, blue);
    node(s, "CN91C4S48\nsetup", 612, 318, 170, 150, cyan);
    node(s, "DDRAM\nwrites", 872, 318, 170, 150, amber);
    text(s, "Startup sequence", 92, 560, 340, 32, { size: 28, bold: true, color: navy });
    text(s, "ICSET -> DISCTL -> EVRSET -> BLKCTL -> APCTL -> MODSET -> Clear RAM", 92, 600, 980, 38, {
      size: 27,
      color: ink,
    });
    label(s, "tested path", 1056, 336, green);
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/hw_i2c.cpp",
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/meter_lcd.cpp",
      "C:/Users/mkoda/Documents/lcd materials/CN91C4S48_i2c.pdf",
    ]);
  }

  {
    const s = p.slides.add();
    s.background.fill = white;
    title(s, "GY1401 PROGRAMMING", "Digits are packed bytes, not ASCII characters", cyan);
    await addImage(s, GY1401_MAP, 704, 210, 480, 330, "GY1401 COM SEG crop", { left: 0.14, top: 0.06, right: 0.04, bottom: 0.18 });
    shape(s, "roundRect", 70, 250, 260, 230, "#E8F7FF", "#B5E3FF", 18);
    shape(s, "roundRect", 370, 250, 260, 230, "#E9FBF8", "#B5EFE8", 18);
    text(s, "LUT", 104, 286, 180, 42, { size: 38, bold: true, color: blue });
    text(s, "0 = FA\n1 = 0A\n8 = FE\n9 = CE", 104, 348, 180, 110, { size: 30, color: ink });
    text(s, "Window", 404, 286, 180, 42, { size: 38, bold: true, color: cyan });
    text(s, "DDRAM 0x18\n11 bytes\nright aligned", 404, 348, 190, 110, { size: 30, color: ink });
    text(s, "Fixed units and icons use the etched COM/SEG map: kWh, Ah, arrows, warning, battery and signal indicators.", 70, 548, 940, 64, {
      size: 30,
      color: navy,
      bold: true,
    });
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/meter_lcd.cpp",
      "C:/Users/mkoda/Documents/lcd materials/lcd mspm0  code/util_ui_lcd_lu.h",
      "C:/Users/mkoda/Documents/lcd materials/COM_SEG Mapping Table.PNG",
    ]);
  }

  {
    const s = p.slides.add();
    s.background.fill = "#FFF8E8";
    title(s, "GY1401 ENERGY METER USE", "The LCD layer should stay separate from meter state logic", amber);
    arrow(s, 318, 372, 64, 38, amber);
    arrow(s, 588, 372, 64, 38, amber);
    arrow(s, 858, 372, 64, 38, amber);
    node(s, "Measurements\nV, A, kW, kWh", 72, 310, 230, 170, white, "#F3CE78", 25);
    node(s, "Formatter\nnumber + unit", 392, 310, 190, 170, white, "#F3CE78", 25);
    node(s, "LCD Driver\nbytes + icons", 662, 310, 190, 170, white, "#F3CE78", 25);
    node(s, "GY1401\nGlass", 932, 310, 190, 170, navy, "none", 26);
    text(s, "This keeps hardware mapping changes out of application logic.", 92, 558, 940, 50, {
      size: 34,
      bold: true,
      color: navy,
    });
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/app/main.cpp",
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/includes/meter_lcd.h",
      "C:/Users/mkoda/Documents/lcd materials/Displayable Characters.txt",
    ]);
  }

  {
    const s = p.slides.add();
    s.background.fill = navy;
    text(s, "GY0801 CONFIGURATION", 64, 38, 500, 28, { size: 18, bold: true, color: "#9DDCFF" });
    text(s, "GY0801 uses GPIO timing for an HT1621-compatible controller", 64, 94, 780, 150, {
      size: 48,
      bold: true,
      color: white,
    });
    await addImage(s, GY0801_MAP, 772, 72, 420, 310, "GY0801 annotated LCD map");
    metric(s, "CS 5", "chip select", 72, 330, blue);
    metric(s, "WR 4", "serial clock", 330, 330, cyan);
    metric(s, "DATA 9", "write data", 588, 330, amber);
    text(s, "Startup enables SYS, RC oscillator, 1/3 bias 4-COM mode, clears RAM, then turns LCD on.", 72, 545, 1030, 70, {
      size: 32,
      bold: true,
      color: "#D9EAFB",
    });
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/app/main.cpp",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/gy0801_lcd.cpp",
      "C:/Users/mkoda/Documents/Premauda/GY0801 LCD Materials/ht1621b.PDF",
    ]);
  }

  {
    const s = p.slides.add();
    s.background.fill = white;
    title(s, "GY0801 PROGRAMMING", "A shadow RAM makes icons and numbers composable", green);
    await addImage(s, GY0801_TRUTH, 742, 202, 430, 356, "GY0801 truth table");
    node(s, "32-address\nshadow RAM", 90, 260, 220, 130, "#EAF8EF", "#BFE8CC", 27);
    node(s, "writeText()\nwriteNumber()", 360, 260, 220, 130, "#E8F7FF", "#B5E3FF", 27);
    node(s, "setIcon()\nsetSymbol()", 90, 430, 220, 130, "#FFF3D8", "#F8D891", 27);
    node(s, "flush()\nburst write", 360, 430, 220, 130, "#F2ECFF", "#D8C6FF", 27);
    text(s, "Digits, decimal points and named icons update the same shadow buffer before a single controller refresh.", 82, 596, 900, 44, {
      size: 30,
      bold: true,
      color: navy,
    });
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/gy0801_lcd.cpp",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/hw/includes/gy0801_lcd.h",
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/tmp/pdf_pages/gy0801_truth_table_current.png",
    ]);
  }

  {
    const s = p.slides.add();
    s.background.fill = "#F4FBFA";
    title(s, "PRO CI DISPLAY STATES", "GY0801 can become the visible state machine", cyan);
    const states = [
      ["IDLE", "#DEE7F2"],
      ["CHARGING", "#DFF8ED"],
      ["DISCHARGING", "#FFF0C9"],
      ["FAULT", "#FFE1E3"],
      ["LOW BATTERY", "#F4E8FF"],
    ];
    states.forEach(([name, color], i) => {
      node(s, name, 70 + i * 226, 256, 185, 88, color, "none", 24);
    });
    text(s, "Each state maps to:", 88, 412, 300, 34, { size: 31, bold: true, color: navy });
    text(s, "numeric regions  |  flow arrows  |  battery gauge  |  warning / overload icons", 88, 464, 1040, 42, {
      size: 31,
      color: ink,
    });
    text(s, "Integration target: a small display adapter that listens to PRO CI state changes and writes only the LCD symbols that changed.", 88, 566, 1000, 68, {
      size: 30,
      bold: true,
      color: navy,
    });
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD/app/main.cpp",
      "User-provided future plan: integrate GY0801 in pro_ci to display different states.",
    ]);
  }

  {
    const s = p.slides.add();
    s.background.fill = navy;
    text(s, "NEXT STEP", 64, 44, 300, 28, { size: 18, bold: true, color: "#9DDCFF" });
    text(s, "Move from LCD bring-up to production display behavior", 64, 106, 840, 116, {
      size: 54,
      bold: true,
      color: white,
    });
    arrow(s, 365, 392, 60, 34, "#77D8FF");
    arrow(s, 665, 392, 60, 34, "#77D8FF");
    node(s, "1\nDefine state map", 86, 322, 240, 170, "#E8F7FF", "none", 27);
    node(s, "2\nCreate adapter", 448, 322, 240, 170, "#E9FBF8", "none", 27);
    node(s, "3\nRefresh on change", 810, 322, 240, 170, "#FFF3D8", "none", 27);
    text(s, "Deliverable: reusable C2000 LCD UI module for energy meter status and measurement display.", 64, 586, 980, 54, {
      size: 32,
      bold: true,
      color: "#D9EAFB",
    });
    addNotes(s, [
      "C:/Users/mkoda/workspace_ccstheia/GY0801_LCD",
      "C:/Users/mkoda/workspace_ccstheia/LCD_I2C",
      "User-provided future plan.",
    ]);
  }

  for (const [i, slide] of p.slides.items.entries()) {
    const stem = `sleek-slide-${String(i + 1).padStart(2, "0")}`;
    const png = await p.export({ slide, format: "png", scale: 1 });
    await fs.writeFile(path.join(TMP, `${stem}.png`), Buffer.from(await png.arrayBuffer()));
  }

  const montage = await p.export({ format: "webp", montage: true, scale: 1 });
  await fs.writeFile(path.join(TMP, "sleek-montage.webp"), Buffer.from(await montage.arrayBuffer()));

  const pptx = await PresentationFile.exportPptx(p);
  await pptx.save(OUT);
}

build().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
