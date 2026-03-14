<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Hotel Management System — README</title>
<link href="https://fonts.googleapis.com/css2?family=DM+Serif+Display&family=DM+Mono:wght@400;500&family=Outfit:wght@300;400;500;600&display=swap" rel="stylesheet">
<style>
  :root {
    --bg:       #0f1117;
    --surface:  #181c27;
    --card:     #1e2335;
    --border:   #2a3050;
    --accent1:  #5b8dee;
    --accent2:  #4ecdc4;
    --accent3:  #f7b731;
    --accent4:  #fd9644;
    --accent5:  #a29bfe;
    --danger:   #fc5c65;
    --green:    #26de81;
    --text:     #e8eaf6;
    --muted:    #8892b0;
    --code-bg:  #141824;
    --radius:   16px;
    --radius-sm:10px;
  }

  * { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    background: var(--bg);
    color: var(--text);
    font-family: 'Outfit', sans-serif;
    font-size: 15px;
    line-height: 1.7;
    min-height: 100vh;
  }

  /* hero */
  .hero {
    background: linear-gradient(135deg, #0f1117 0%, #1a1f35 50%, #0f1117 100%);
    border-bottom: 1px solid var(--border);
    padding: 64px 48px 56px;
    position: relative;
    overflow: hidden;
  }
  .hero::before {
    content: '';
    position: absolute;
    top: -120px; right: -120px;
    width: 400px; height: 400px;
    border-radius: 50%;
    background: radial-gradient(circle, rgba(91,141,238,0.10) 0%, transparent 70%);
    pointer-events: none;
  }
  .hero::after {
    content: '';
    position: absolute;
    bottom: -80px; left: 200px;
    width: 300px; height: 300px;
    border-radius: 50%;
    background: radial-gradient(circle, rgba(78,205,196,0.07) 0%, transparent 70%);
    pointer-events: none;
  }
  .hero-tag {
    display: inline-flex; align-items: center; gap: 8px;
    background: rgba(91,141,238,0.12);
    border: 1px solid rgba(91,141,238,0.28);
    border-radius: 999px;
    padding: 4px 14px;
    font-size: 11px; font-weight: 500; letter-spacing: .08em;
    color: var(--accent1); text-transform: uppercase;
    margin-bottom: 20px;
  }
  .hero-tag::before { content: ''; width: 7px; height: 7px; border-radius: 50%; background: var(--accent1); animation: pulse 2s infinite; }
  @keyframes pulse { 0%,100%{opacity:1;transform:scale(1)} 50%{opacity:.5;transform:scale(1.4)} }
  h1 {
    font-family: 'DM Serif Display', serif;
    font-size: 52px; font-weight: 400; line-height: 1.1;
    color: #fff; margin-bottom: 16px;
  }
  h1 span { color: var(--accent2); }
  .hero-sub {
    font-size: 16px; color: var(--muted); max-width: 580px; line-height: 1.65;
    margin-bottom: 32px;
  }
  .badge-row { display: flex; flex-wrap: wrap; gap: 10px; }
  .badge {
    padding: 5px 14px; border-radius: 999px; font-size: 12px; font-weight: 500;
    border: 1px solid;
  }
  .badge-blue  { background: rgba(91,141,238,.12); border-color: rgba(91,141,238,.30); color: var(--accent1); }
  .badge-teal  { background: rgba(78,205,196,.12); border-color: rgba(78,205,196,.30); color: var(--accent2); }
  .badge-amber { background: rgba(247,183,49,.12); border-color: rgba(247,183,49,.30); color: var(--accent3); }
  .badge-green { background: rgba(38,222,129,.12); border-color: rgba(38,222,129,.30); color: var(--green); }
  .badge-purple{ background: rgba(162,155,254,.12);border-color: rgba(162,155,254,.30);color: var(--accent5); }

  /* layout */
  .container { max-width: 960px; margin: 0 auto; padding: 48px 32px; }

  /* section headers */
  h2 {
    font-family: 'DM Serif Display', serif;
    font-size: 28px; font-weight: 400; color: #fff;
    margin: 56px 0 24px;
    display: flex; align-items: center; gap: 12px;
  }
  h2::after { content: ''; flex: 1; height: 1px; background: var(--border); }
  h3 { font-size: 15px; font-weight: 600; color: var(--text); margin: 24px 0 12px; }

  /* cards */
  .card {
    background: var(--card);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 24px 28px;
    margin-bottom: 16px;
    transition: border-color .2s, box-shadow .2s;
  }
  .card:hover { border-color: rgba(91,141,238,.35); box-shadow: 0 4px 24px rgba(0,0,0,.25); }

  /* class block */
  .class-block {
    background: var(--code-bg);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    overflow: hidden;
    margin-bottom: 20px;
  }
  .class-header {
    padding: 12px 20px;
    display: flex; align-items: center; gap: 12px;
    border-bottom: 1px solid var(--border);
  }
  .class-dot { width: 10px; height: 10px; border-radius: 50%; flex-shrink: 0; }
  .class-name {
    font-family: 'DM Mono', monospace; font-size: 15px; font-weight: 500; color: #fff;
  }
  .class-stereotype {
    font-size: 11px; letter-spacing: .08em; text-transform: uppercase;
    padding: 2px 10px; border-radius: 999px; border: 1px solid; font-weight: 500;
  }

  /* table */
  table { width: 100%; border-collapse: collapse; font-size: 13px; }
  thead tr { border-bottom: 1px solid var(--border); }
  th { padding: 10px 14px; text-align: left; color: var(--muted); font-weight: 500; font-size: 11px; letter-spacing: .06em; text-transform: uppercase; }
  td { padding: 9px 14px; border-bottom: 1px solid rgba(42,48,80,.5); vertical-align: top; }
  tr:last-child td { border-bottom: none; }
  tr:hover td { background: rgba(255,255,255,.02); }
  .vis { font-family: 'DM Mono', monospace; font-size: 13px; font-weight: 500; }
  .vis-priv { color: var(--danger); }
  .vis-prot { color: var(--accent3); }
  .vis-pub  { color: var(--green); }
  code {
    font-family: 'DM Mono', monospace; font-size: 12.5px;
    background: rgba(91,141,238,.10); border: 1px solid rgba(91,141,238,.18);
    border-radius: 5px; padding: 1px 7px; color: var(--accent1);
  }

  /* code block */
  pre {
    background: var(--code-bg);
    border: 1px solid var(--border);
    border-radius: var(--radius-sm);
    padding: 20px 24px;
    overflow-x: auto;
    font-family: 'DM Mono', monospace; font-size: 13px; line-height: 1.7;
    color: #cdd6f4;
    margin: 12px 0 20px;
  }
  .kw  { color: var(--accent5); }
  .cm  { color: var(--muted); font-style: italic; }
  .str { color: var(--accent2); }
  .num { color: var(--accent3); }
  .cls { color: var(--accent1); }

  /* subtype grid */
  .subtype-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 14px; margin-bottom: 16px; }
  @media(max-width:640px){ .subtype-grid { grid-template-columns: 1fr; } }

  .subtype-card {
    border-radius: var(--radius);
    border: 1px solid var(--border);
    overflow: hidden;
  }
  .subtype-head {
    padding: 12px 16px 10px;
    border-bottom: 1px solid var(--border);
    display: flex; align-items: center; justify-content: space-between;
  }
  .subtype-title { font-family: 'DM Mono', monospace; font-size: 14px; font-weight: 500; }
  .subtype-body { padding: 14px 16px; }
  .subtype-body p { font-size: 12px; color: var(--muted); margin-bottom: 10px; line-height: 1.5; }
  .method-list { list-style: none; }
  .method-list li {
    font-family: 'DM Mono', monospace; font-size: 12px;
    color: var(--text); padding: 3px 0;
    display: flex; gap: 8px;
  }
  .method-list li .vis { font-size: 11px; }

  /* Solo */
  .s-solo   { background: rgba(91,141,238,.06); }
  .h-solo   { background: rgba(91,141,238,.10); }
  .dot-solo { background: var(--accent1); }
  /* Family */
  .s-family { background: rgba(38,222,129,.06); }
  .h-family { background: rgba(38,222,129,.10); }
  .dot-family { background: var(--green); }
  /* Group */
  .s-group  { background: rgba(247,183,49,.06); }
  .h-group  { background: rgba(247,183,49,.10); }
  .dot-group { background: var(--accent3); }
  /* Couple */
  .s-couple { background: rgba(253,150,68,.06); }
  .h-couple { background: rgba(253,150,68,.10); }
  .dot-couple { background: var(--accent4); }

  /* arch diagram */
  .arch {
    background: var(--code-bg);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    padding: 28px 32px;
    font-family: 'DM Mono', monospace; font-size: 13px; line-height: 1.8;
    color: #cdd6f4; margin-bottom: 16px; overflow-x: auto;
  }
  .arch .c1 { color: var(--accent1); }
  .arch .c2 { color: var(--accent2); }
  .arch .c3 { color: var(--accent3); }
  .arch .c4 { color: var(--green); }
  .arch .cm { color: var(--muted); }

  /* legend */
  .legend-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 12px; }
  @media(max-width:640px){ .legend-grid { grid-template-columns: 1fr; } }
  .legend-item {
    background: var(--card); border: 1px solid var(--border);
    border-radius: var(--radius-sm);
    padding: 14px 16px;
    display: flex; align-items: flex-start; gap: 12px;
  }
  .legend-sym {
    font-family: 'DM Mono', monospace; font-size: 18px; font-weight: 500;
    min-width: 28px; text-align: center; line-height: 1.3;
  }
  .legend-desc { font-size: 12px; color: var(--muted); line-height: 1.5; }
  .legend-desc strong { color: var(--text); font-weight: 500; display: block; margin-bottom: 2px; }

  /* enum */
  .enum-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 12px; }
  @media(max-width:640px){ .enum-grid { grid-template-columns: 1fr; } }
  .enum-card {
    background: var(--card); border: 1px solid var(--border);
    border-radius: var(--radius-sm); overflow: hidden;
  }
  .enum-head {
    background: rgba(255,255,255,.03); border-bottom: 1px solid var(--border);
    padding: 8px 14px;
    font-family: 'DM Mono', monospace; font-size: 12px; font-weight: 500;
  }
  .enum-values { padding: 10px 14px; display: flex; flex-wrap: wrap; gap: 6px; }
  .enum-val {
    background: rgba(91,141,238,.10); border: 1px solid rgba(91,141,238,.20);
    border-radius: 6px; padding: 2px 10px;
    font-family: 'DM Mono', monospace; font-size: 11px; color: var(--accent1);
  }

  /* build */
  .file-tree {
    background: var(--code-bg); border: 1px solid var(--border);
    border-radius: var(--radius-sm); padding: 20px 24px;
    font-family: 'DM Mono', monospace; font-size: 13px; line-height: 1.9;
    color: #cdd6f4;
  }
  .ft-dir  { color: var(--accent2); }
  .ft-file { color: var(--muted); }
  .ft-main { color: var(--accent1); }

  footer {
    border-top: 1px solid var(--border);
    text-align: center; padding: 32px;
    font-size: 12px; color: var(--muted);
  }

  p { margin-bottom: 12px; }
  p:last-child { margin-bottom: 0; }
</style>
</head>
<body>

<!-- ═══════════ HERO ═══════════ -->
<div class="hero">
  <div class="hero-tag">C++23 · OOP · Domain-Driven Design</div>
  <h1>Hotel Management<br><span>System</span></h1>
  <p class="hero-sub">
    Object-oriented reservation platform built with full class hierarchy,
    dynamic pricing, amenity management, and a clean separation between
    domain model and application logic.
  </p>
  <div class="badge-row">
    <span class="badge badge-blue">C++23</span>
    <span class="badge badge-teal">5 Core Classes</span>
    <span class="badge badge-amber">4 Reservation Types</span>
    <span class="badge badge-green">6 Enumerations</span>
    <span class="badge badge-purple">Inheritance + Composition</span>
  </div>
</div>

<div class="container">

  <!-- ═══════════ ARCHITECTURE ═══════════ -->
  <h2>Architecture Overview</h2>
  <div class="arch">
<span class="c1">Room</span>  ──aggregation──▷  <span class="c2">Reservation</span>  ──inheritance──▷  <span class="c3">Solo</span>
                              <span class="cm">          ├──inheritance──▷</span>  <span class="c4">Family</span>
                              <span class="cm">          ├──inheritance──▷</span>  <span class="c3">Group</span>
                              <span class="cm">          └──inheritance──▷</span>  <span class="c3">Couple</span>

<span class="c1">Hotel</span> ──composition──◇  <span class="c2">Amenity</span>
<span class="c1">Hotel</span> ──manages──────▷  <span class="c2">Reservation*</span>   <span class="cm">// polymorphic vector</span>
<span class="c1">Hotel</span> ──manages──────▷  <span class="c2">Room</span>
  </div>

  <!-- ═══════════ ROOM ═══════════ -->
  <h2>Class Reference</h2>

  <div class="class-block">
    <div class="class-header">
      <div class="class-dot" style="background:var(--accent2)"></div>
      <span class="class-name">Room</span>
      <span class="class-stereotype badge-teal" style="border-color:rgba(78,205,196,.3);color:var(--accent2);font-size:11px;padding:2px 10px;border-radius:999px;border:1px solid;letter-spacing:.06em">entity</span>
    </div>
    <table>
      <thead><tr><th>Vis</th><th>Member</th><th>Type</th><th>Description</th></tr></thead>
      <tbody>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>number</code></td><td><code>int</code></td><td>Unique room identifier</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>capacity</code></td><td><code>int</code></td><td>Maximum guest count</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>type</code></td><td><code>RoomCategory</code></td><td>Standard / Deluxe / Suite / Penthouse</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>status</code></td><td><code>RoomStatus</code></td><td>Available / Occupied / Maintenance</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>Room(n, c, type)</code></td><td>constructor</td><td>Initialise with number, capacity, category</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>getNumber()</code></td><td><code>int</code></td><td>Returns room number</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>getCapacity()</code></td><td><code>int</code></td><td>Returns max capacity</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>getType()</code></td><td><code>string</code></td><td>Returns category label</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>getStatus()</code></td><td><code>string</code></td><td>Returns current status string</td></tr>
      </tbody>
    </table>
  </div>

  <!-- ═══════════ RESERVATION ═══════════ -->
  <div class="class-block">
    <div class="class-header">
      <div class="class-dot" style="background:var(--accent1)"></div>
      <span class="class-name">Reservation</span>
      <span class="class-stereotype" style="background:rgba(91,141,238,.10);border:1px solid rgba(91,141,238,.28);color:var(--accent1);font-size:11px;padding:2px 10px;border-radius:999px;letter-spacing:.06em">abstract</span>
    </div>
    <table>
      <thead><tr><th>Vis</th><th>Member</th><th>Type</th><th>Description</th></tr></thead>
      <tbody>
        <tr><td><span class="vis vis-prot">#</span></td><td><code>name</code></td><td><code>string</code></td><td>Guest full name</td></tr>
        <tr><td><span class="vis vis-prot">#</span></td><td><code>room</code></td><td><code>int</code></td><td>Assigned room number</td></tr>
        <tr><td><span class="vis vis-prot">#</span></td><td><code>date</code></td><td><code>string</code></td><td>Check-in date (ISO 8601)</td></tr>
        <tr><td><span class="vis vis-prot">#</span></td><td><code>discount</code></td><td><code>double</code></td><td>Applied discount 0.0 – 1.0</td></tr>
        <tr><td><span class="vis vis-prot">#</span></td><td><code>groupType</code></td><td><code>GuestGroupType</code></td><td>Solo / Couple / Family / Group / Corporate</td></tr>
        <tr><td><span class="vis vis-prot">#</span></td><td><code>mealPlan</code></td><td><code>MealPlan</code></td><td>RoomOnly / BB / HalfBoard / FullBoard / AI</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>Reservation(n, r, d)</code></td><td>constructor</td><td>—</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>reservationType()</code></td><td><code>string</code></td><td><strong>Pure virtual</strong> — overridden by each subclass</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>calculatePrice()</code></td><td><code>double</code></td><td>Base price × nights × meal plan modifier</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>addDiscount(d)</code></td><td><code>void</code></td><td>Apply percentage discount to booking</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>display()</code></td><td><code>void</code></td><td>Print full reservation summary</td></tr>
      </tbody>
    </table>
  </div>

  <!-- ═══════════ SUBTYPES ═══════════ -->
  <h3>Reservation Subtypes — Inheritance</h3>
  <div class="subtype-grid">

    <div class="subtype-card s-solo">
      <div class="subtype-head h-solo">
        <span class="subtype-title" style="color:var(--accent1)">Solo</span>
        <span class="badge badge-blue" style="font-size:11px;padding:2px 10px">single traveller</span>
      </div>
      <div class="subtype-body">
        <p>Single adult. Unlocks business packages, late check-out perks, and workspace upgrades.</p>
        <ul class="method-list">
          <li><span class="vis vis-priv">−</span> <code>age: int</code></li>
          <li><span class="vis vis-pub">+</span> <code>reservationType()</code> → <em>"Solo"</em></li>
          <li><span class="vis vis-pub">+</span> <code>businessPackage()</code></li>
        </ul>
      </div>
    </div>

    <div class="subtype-card s-family">
      <div class="subtype-head h-family">
        <span class="subtype-title" style="color:var(--green)">Family</span>
        <span class="badge badge-green" style="font-size:11px;padding:2px 10px">adults + children</span>
      </div>
      <div class="subtype-body">
        <p>Two or more adults with children. Triggers cot availability, safety checks, and kids-meal pricing.</p>
        <ul class="method-list">
          <li><span class="vis vis-priv">−</span> <code>adults: int</code></li>
          <li><span class="vis vis-priv">−</span> <code>children: int</code></li>
          <li><span class="vis vis-pub">+</span> <code>reservationType()</code> → <em>"Family"</em></li>
          <li><span class="vis vis-pub">+</span> <code>familyPackage()</code></li>
          <li><span class="vis vis-pub">+</span> <code>addCrib()</code></li>
        </ul>
      </div>
    </div>

    <div class="subtype-card s-group">
      <div class="subtype-head h-group">
        <span class="subtype-title" style="color:var(--accent3)">Group</span>
        <span class="badge badge-amber" style="font-size:11px;padding:2px 10px">10+ people</span>
      </div>
      <div class="subtype-body">
        <p>Ten or more guests. Activates group rate pricing tier and stores an organiser contact.</p>
        <ul class="method-list">
          <li><span class="vis vis-priv">−</span> <code>numberOfPersons: int</code></li>
          <li><span class="vis vis-priv">−</span> <code>organiser: string</code></li>
          <li><span class="vis vis-pub">+</span> <code>reservationType()</code> → <em>"Group"</em></li>
          <li><span class="vis vis-pub">+</span> <code>groupRate()</code></li>
          <li><span class="vis vis-pub">+</span> <code>getOrganiser()</code></li>
        </ul>
      </div>
    </div>

    <div class="subtype-card s-couple">
      <div class="subtype-head h-couple">
        <span class="subtype-title" style="color:var(--accent4)">Couple</span>
        <span class="badge badge-amber" style="background:rgba(253,150,68,.12);border-color:rgba(253,150,68,.30);color:var(--accent4);font-size:11px;padding:2px 10px">two adults</span>
      </div>
      <div class="subtype-body">
        <p>Two adults. Unlocks romantic packages, champagne on arrival, and hotel-arranged surprises.</p>
        <ul class="method-list">
          <li><span class="vis vis-priv">−</span> <code>romantic: bool</code></li>
          <li><span class="vis vis-pub">+</span> <code>reservationType()</code> → <em>"Couple"</em></li>
          <li><span class="vis vis-pub">+</span> <code>romanticPackage()</code></li>
          <li><span class="vis vis-pub">+</span> <code>addSurprise()</code></li>
        </ul>
      </div>
    </div>
  </div>

  <!-- ═══════════ HOTEL ═══════════ -->
  <div class="class-block">
    <div class="class-header">
      <div class="class-dot" style="background:var(--accent5)"></div>
      <span class="class-name">Hotel</span>
      <span class="class-stereotype" style="background:rgba(162,155,254,.10);border:1px solid rgba(162,155,254,.28);color:var(--accent5);font-size:11px;padding:2px 10px;border-radius:999px;letter-spacing:.06em">controller</span>
    </div>
    <table>
      <thead><tr><th>Vis</th><th>Method</th><th>Returns</th><th>Description</th></tr></thead>
      <tbody>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>reservations</code></td><td><code>vector&lt;Reservation*&gt;</code></td><td>Polymorphic collection of all bookings</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>rooms</code></td><td><code>vector&lt;Room&gt;</code></td><td>All registered rooms</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>amenities</code></td><td><code>vector&lt;Amenity&gt;</code></td><td>Spa, pool, gym, parking…</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>pricelist</code></td><td><code>map&lt;MealPlan, double&gt;</code></td><td>Per-plan daily price modifier</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>isAvailable(room, date)</code></td><td><code>bool</code></td><td>Check if room is free on a given date</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>checkAvailability()</code></td><td><code>void</code></td><td>Print full room availability grid</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>addReservation(r)</code></td><td><code>void</code></td><td>Register a new reservation pointer</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>refuseReservation(reason)</code></td><td><code>void</code></td><td>Reject with logged reason string</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>addDiscount(resId, d)</code></td><td><code>void</code></td><td>Apply discount to existing booking</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>showReservations()</code></td><td><code>void</code></td><td>List all active reservations</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>checkIn(resId)</code></td><td><code>void</code></td><td>Mark guest as arrived, update room status</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>checkOut(resId)</code></td><td><code>void</code></td><td>Finalise stay, free room, generate invoice</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>addAmenity(a)</code></td><td><code>void</code></td><td>Register spa / pool / gym service</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>calculateTotalPrice(resId)</code></td><td><code>double</code></td><td>Room + meal plan + amenities total</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>generateReport()</code></td><td><code>void</code></td><td>Occupancy and revenue summary</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>findReservation(name)</code></td><td><code>Reservation*</code></td><td>Search by guest name</td></tr>
      </tbody>
    </table>
  </div>

  <!-- ═══════════ AMENITY ═══════════ -->
  <div class="class-block">
    <div class="class-header">
      <div class="class-dot" style="background:var(--accent4)"></div>
      <span class="class-name">Amenity</span>
      <span class="class-stereotype" style="background:rgba(253,150,68,.10);border:1px solid rgba(253,150,68,.28);color:var(--accent4);font-size:11px;padding:2px 10px;border-radius:999px;letter-spacing:.06em">composition</span>
    </div>
    <table>
      <thead><tr><th>Vis</th><th>Member</th><th>Type</th><th>Description</th></tr></thead>
      <tbody>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>name</code></td><td><code>string</code></td><td>Display name, e.g. "Rooftop Spa"</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>type</code></td><td><code>AmenityType</code></td><td>Spa / Pool / Gym / Parking / Transfer</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>price</code></td><td><code>double</code></td><td>Base price in local currency</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>available</code></td><td><code>bool</code></td><td>Whether bookings are currently open</td></tr>
        <tr><td><span class="vis vis-priv">−</span></td><td><code>pricingModel</code></td><td><code>PricingModel</code></td><td>Free / PerVisit / PerDay / PerPerson</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>Amenity(n, t, p)</code></td><td>constructor</td><td>—</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>getName()</code></td><td><code>string</code></td><td>—</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>getPrice()</code></td><td><code>double</code></td><td>—</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>isAvailable()</code></td><td><code>bool</code></td><td>—</td></tr>
        <tr><td><span class="vis vis-pub">+</span></td><td><code>setAvailable(a)</code></td><td><code>void</code></td><td>Open or close bookings for this service</td></tr>
      </tbody>
    </table>
  </div>

  <!-- ═══════════ ENUMS ═══════════ -->
  <h2>Enumeration Types</h2>
  <div class="enum-grid">
    <div class="enum-card">
      <div class="enum-head" style="color:var(--accent2)">RoomCategory</div>
      <div class="enum-values">
        <span class="enum-val">Standard</span><span class="enum-val">Deluxe</span>
        <span class="enum-val">Suite</span><span class="enum-val">Penthouse</span>
      </div>
    </div>
    <div class="enum-card">
      <div class="enum-head" style="color:var(--danger)">RoomStatus</div>
      <div class="enum-values" style="gap:6px">
        <span class="enum-val" style="background:rgba(252,92,101,.10);border-color:rgba(252,92,101,.25);color:var(--danger)">Available</span>
        <span class="enum-val" style="background:rgba(252,92,101,.10);border-color:rgba(252,92,101,.25);color:var(--danger)">Occupied</span>
        <span class="enum-val" style="background:rgba(252,92,101,.10);border-color:rgba(252,92,101,.25);color:var(--danger)">Maintenance</span>
      </div>
    </div>
    <div class="enum-card">
      <div class="enum-head" style="color:var(--accent1)">GuestGroupType</div>
      <div class="enum-values">
        <span class="enum-val">Solo</span><span class="enum-val">Couple</span>
        <span class="enum-val">Family</span><span class="enum-val">Group</span>
        <span class="enum-val">Corporate</span>
      </div>
    </div>
    <div class="enum-card">
      <div class="enum-head" style="color:var(--green)">MealPlan</div>
      <div class="enum-values" style="gap:6px">
        <span class="enum-val" style="background:rgba(38,222,129,.10);border-color:rgba(38,222,129,.25);color:var(--green)">RoomOnly</span>
        <span class="enum-val" style="background:rgba(38,222,129,.10);border-color:rgba(38,222,129,.25);color:var(--green)">BB</span>
        <span class="enum-val" style="background:rgba(38,222,129,.10);border-color:rgba(38,222,129,.25);color:var(--green)">HalfBoard</span>
        <span class="enum-val" style="background:rgba(38,222,129,.10);border-color:rgba(38,222,129,.25);color:var(--green)">FullBoard</span>
        <span class="enum-val" style="background:rgba(38,222,129,.10);border-color:rgba(38,222,129,.25);color:var(--green)">AllInclusive</span>
      </div>
    </div>
    <div class="enum-card">
      <div class="enum-head" style="color:var(--accent4)">AmenityType</div>
      <div class="enum-values" style="gap:6px">
        <span class="enum-val" style="background:rgba(253,150,68,.10);border-color:rgba(253,150,68,.25);color:var(--accent4)">Spa</span>
        <span class="enum-val" style="background:rgba(253,150,68,.10);border-color:rgba(253,150,68,.25);color:var(--accent4)">Pool</span>
        <span class="enum-val" style="background:rgba(253,150,68,.10);border-color:rgba(253,150,68,.25);color:var(--accent4)">Gym</span>
        <span class="enum-val" style="background:rgba(253,150,68,.10);border-color:rgba(253,150,68,.25);color:var(--accent4)">Parking</span>
        <span class="enum-val" style="background:rgba(253,150,68,.10);border-color:rgba(253,150,68,.25);color:var(--accent4)">Transfer</span>
      </div>
    </div>
    <div class="enum-card">
      <div class="enum-head" style="color:var(--accent5)">PricingModel</div>
      <div class="enum-values" style="gap:6px">
        <span class="enum-val" style="background:rgba(162,155,254,.10);border-color:rgba(162,155,254,.25);color:var(--accent5)">Free</span>
        <span class="enum-val" style="background:rgba(162,155,254,.10);border-color:rgba(162,155,254,.25);color:var(--accent5)">PerVisit</span>
        <span class="enum-val" style="background:rgba(162,155,254,.10);border-color:rgba(162,155,254,.25);color:var(--accent5)">PerDay</span>
        <span class="enum-val" style="background:rgba(162,155,254,.10);border-color:rgba(162,155,254,.25);color:var(--accent5)">PerPerson</span>
      </div>
    </div>
  </div>

  <!-- ═══════════ LEGEND ═══════════ -->
  <h2>Symbol Legend</h2>
  <div class="legend-grid">
    <div class="legend-item">
      <span class="legend-sym" style="color:var(--accent1)">△</span>
      <div class="legend-desc"><strong>Inheritance</strong>Derived class extends base. Child inherits all public and protected members.</div>
    </div>
    <div class="legend-item">
      <span class="legend-sym" style="color:var(--accent4)">◇</span>
      <div class="legend-desc"><strong>Composition</strong>Child cannot exist without parent. Amenity is destroyed when Hotel is destroyed.</div>
    </div>
    <div class="legend-item">
      <span class="legend-sym" style="color:var(--accent2)">→</span>
      <div class="legend-desc"><strong>Aggregation</strong>Loose association. Room can exist independently of any Reservation.</div>
    </div>
    <div class="legend-item">
      <span class="legend-sym vis-priv">−</span>
      <div class="legend-desc"><strong>Private</strong>Accessible within the declaring class only.</div>
    </div>
    <div class="legend-item">
      <span class="legend-sym vis-prot">#</span>
      <div class="legend-desc"><strong>Protected</strong>Accessible by the class and all derived subclasses.</div>
    </div>
    <div class="legend-item">
      <span class="legend-sym vis-pub">+</span>
      <div class="legend-desc"><strong>Public</strong>Accessible from anywhere in the program.</div>
    </div>
  </div>

  <!-- ═══════════ BUILD ═══════════ -->
  <h2>Build & File Structure</h2>

  <pre><span class="kw">g++</span> -std=c++23 -Wall -Wextra -o hotel main.cpp
<span class="kw">./hotel</span>

<span class="cm"># or with CMake</span>
<span class="kw">cmake</span> -B build -DCMAKE_BUILD_TYPE=Release
<span class="kw">cmake</span> --build build
./build/hotel</pre>

  <div class="file-tree">
<span class="ft-dir">hotel-system/</span>
├── <span class="ft-dir">src/</span>
│   ├── <span class="ft-dir">domain/</span>
│   │   ├── <span class="ft-file">Room.hpp</span>
│   │   ├── <span class="ft-file">Reservation.hpp</span>
│   │   ├── <span class="ft-file">Solo.hpp</span>
│   │   ├── <span class="ft-file">Family.hpp</span>
│   │   ├── <span class="ft-file">Group.hpp</span>
│   │   ├── <span class="ft-file">Couple.hpp</span>
│   │   └── <span class="ft-file">Amenity.hpp</span>
│   ├── <span class="ft-file">Hotel.hpp</span>
│   ├── <span class="ft-file">Hotel.cpp</span>
│   └── <span class="ft-main">main.cpp</span>
├── <span class="ft-dir">tests/</span>
│   └── <span class="ft-file">hotel_tests.cpp</span>
├── <span class="ft-file">CMakeLists.txt</span>
└── <span class="ft-main">README.md</span>
  </div>

</div>

<footer>
  Hotel Management System · C++23 · MIT License
</footer>

</body>
</html>
