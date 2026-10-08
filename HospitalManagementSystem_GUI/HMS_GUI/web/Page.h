#pragma once
static const char* kPage = R"HMSPAGE(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Hospital Management System</title>
<style>
/* style.css - all styling
   Owner: Sushant */

/* ===== Colours ===== */
:root {
  --bg: #f2f4f7;
  --surface: #ffffff;
  --text: #1f2933;
  --muted: #66788a;
  --label: #3e4c59;
  --border: #d9e0e6;
  --line: #e6ebef;
  --navy: #1c3d5a;
  --primary: #2b6a99;
  --primary-dark: #215377;
  --primary-soft: #e8f0f7;
  --danger: #b23b3b;
  --warning: #b9770e;
  --success: #2c6b4a;
}

/* ===== Base ===== */
* {
  box-sizing: border-box;
}

body {
  margin: 0;
  font-family: "Segoe UI", system-ui, -apple-system, "Helvetica Neue", Arial, sans-serif;
  font-size: 14px;
  line-height: 1.45;
  background: var(--bg);
  color: var(--text);
}

button, input, select, textarea {
  font: inherit;
}

h3 {
  font-weight: 600;
}

.hide {
  display: none !important;
}

/* ===== Forms ===== */
label {
  display: block;
  margin: 14px 0 4px;
  font-size: 13px;
  font-weight: 600;
  color: var(--label);
}

input, select, textarea {
  width: 100%;
  padding: 7px 10px;
  border: 1px solid #c5ced6;
  border-radius: 3px;
  background: #fff;
  color: var(--text);
}

input:focus, select:focus, textarea:focus {
  outline: none;
  border-color: var(--primary);
  box-shadow: 0 0 0 2px rgba(43, 106, 153, 0.18);
}

textarea {
  min-height: 70px;
  resize: vertical;
}

.two {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 14px;
}

.err {
  min-height: 18px;
  margin-top: 10px;
  font-size: 13px;
  color: var(--danger);
}

/* ===== Buttons ===== */
.btn {
  padding: 7px 14px;
  border: 1px solid var(--primary);
  border-radius: 3px;
  background: var(--primary);
  color: #fff;
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
}

.btn:hover       { background: var(--primary-dark); border-color: var(--primary-dark); }
.btn.ghost       { background: #fff; border-color: #c5ced6; color: var(--label); }
.btn.ghost:hover { background: #f1f4f6; }
.btn.red         { background: var(--danger); border-color: var(--danger); }
.btn.red:hover   { background: #962f2f; border-color: #962f2f; }
.btn.warn        { background: var(--warning); border-color: var(--warning); }
.btn.warn:hover  { background: #9a6309; border-color: #9a6309; }
.btn.sm          { padding: 3px 9px; font-size: 12px; }
.btn.full        { width: 100%; margin-top: 18px; padding: 9px; }

/* ===== Login ===== */
#login {
  display: flex;
  align-items: center;
  justify-content: center;
  min-height: 100vh;
  background: #e4e9ee;
}

.lcard {
  width: 380px;
  border: 1px solid var(--border);
  border-radius: 4px;
  background: #fff;
  overflow: hidden;
}

.lhead {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 18px 24px;
  background: var(--navy);
  color: #fff;
}

.lhead b     { display: block; font-size: 16px; font-weight: 600; }
.lhead small { color: #b7c7d6; }
.lbody       { padding: 8px 24px 22px; }

.hint {
  margin-top: 18px;
  padding-top: 12px;
  border-top: 1px solid var(--line);
  font-size: 12px;
  color: var(--muted);
}

/* ===== Top bar ===== */
.bar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  height: 48px;
  padding: 0 20px;
  background: var(--navy);
  color: #fff;
}

.bar-brand {
  display: flex;
  align-items: center;
  gap: 10px;
  font-size: 15px;
  font-weight: 600;
}

.bar-user       { font-size: 13px; color: #d5e0ea; }
.bar-user b     { color: #fff; font-weight: 600; }
.bar-user span  { margin-left: 8px; padding: 1px 8px; border-radius: 3px; background: rgba(255, 255, 255, 0.14); font-size: 12px; }

/* ===== Layout: sidebar + main ===== */
.shell {
  display: flex;
  min-height: calc(100vh - 48px);
}

aside {
  display: flex;
  flex-direction: column;
  width: 224px;
  border-right: 1px solid var(--border);
  background: #fff;
}

nav {
  flex: 1;
  padding: 10px 0;
}

nav button, .lo {
  display: flex;
  align-items: center;
  gap: 10px;
  width: 100%;
  padding: 10px 18px;
  border: 0;
  border-left: 3px solid transparent;
  background: none;
  color: var(--label);
  font-size: 14px;
  text-align: left;
  cursor: pointer;
}

nav button:hover, .lo:hover { background: #f1f4f7; }

nav button.on {
  border-left-color: var(--primary);
  background: var(--primary-soft);
  color: #1f4e79;
  font-weight: 600;
}

.lo {
  border-top: 1px solid var(--line);
  color: var(--muted);
}

.ic {
  width: 17px;
  height: 17px;
  flex: none;
}

main {
  flex: 1;
  min-width: 0;
  padding: 22px 28px;
}

.top {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 18px;
  padding-bottom: 12px;
  border-bottom: 1px solid var(--border);
}

.top h2 {
  margin: 0;
  font-size: 20px;
  font-weight: 600;
  color: var(--navy);
}

.tools {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 10px;
}

.tools input, .tools select { width: 240px; }

.lead {
  margin: 0 0 16px;
  color: var(--muted);
}

/* ===== Cards & stats ===== */
.card {
  margin-bottom: 18px;
  padding: 16px 18px;
  border: 1px solid var(--border);
  border-radius: 4px;
  background: var(--surface);
}

.cards {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(170px, 1fr));
  gap: 14px;
  margin-bottom: 18px;
}

.stat {
  padding: 14px 16px;
  border: 1px solid var(--border);
  border-radius: 4px;
  background: #fff;
}

.stat .n { font-size: 26px; font-weight: 600; color: var(--navy); }
.stat .l { font-size: 13px; color: var(--muted); }

.empty {
  padding: 28px;
  text-align: center;
  color: var(--muted);
}

/* ===== Tables ===== */
table {
  width: 100%;
  border: 1px solid var(--border);
  border-collapse: collapse;
  background: #fff;
}

th {
  padding: 9px 12px;
  border-bottom: 1px solid var(--border);
  background: #eef2f5;
  text-align: left;
  font-size: 13px;
  font-weight: 600;
  color: var(--label);
}

td {
  padding: 9px 12px;
  border-bottom: 1px solid var(--line);
}

tr:last-child td { border-bottom: 0; }
tr:hover td      { background: #f8fafb; }

.act {
  display: flex;
  flex-wrap: wrap;
  gap: 5px;
}

/* ===== Status tags ===== */
.tag {
  display: inline-block;
  padding: 1px 8px;
  border-radius: 3px;
  font-size: 12px;
  font-weight: 600;
}

.Scheduled { background: #e3eef8; color: #1f5a8a; }
.Completed,
.Paid      { background: #e1f0e8; color: var(--success); }
.Cancelled { background: #f6e3e3; color: #9a3030; }
.Unpaid    { background: #fbefd9; color: #8f5b0a; }

/* ===== Dialog & message bar ===== */
.ov {
  position: fixed;
  inset: 0;
  z-index: 50;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 16px;
  background: rgba(20, 35, 50, 0.45);
}

.md {
  width: 520px;
  max-width: 100%;
  max-height: 92vh;
  border: 1px solid #b9c4cd;
  border-radius: 4px;
  background: #fff;
  overflow: auto;
}

.md h3 {
  margin: 0;
  padding: 12px 20px;
  border-bottom: 1px solid var(--border);
  background: #eef2f5;
  font-size: 15px;
  color: var(--navy);
}

.md > :not(h3):not(.row) {
  width: calc(100% - 40px);
  margin-left: 20px;
  margin-right: 20px;
}

.md .row {
  display: flex;
  justify-content: flex-end;
  gap: 8px;
  margin-top: 16px;
  padding: 12px 20px;
  border-top: 1px solid var(--border);
  background: #f6f8fa;
}

#toast {
  position: fixed;
  right: 22px;
  bottom: 22px;
  z-index: 99;
  padding: 10px 16px;
  border-radius: 3px;
  background: var(--navy);
  color: #fff;
}

#toast.bad { background: var(--danger); }

/* ===== Medical history timeline ===== */
.tl {
  margin-left: 6px;
  padding-left: 20px;
  border-left: 2px solid #c5d3df;
}

.visit {
  position: relative;
  margin-bottom: 12px;
  padding: 14px 16px;
  border: 1px solid var(--border);
  border-radius: 4px;
  background: #fff;
}

.visit::before {
  content: "";
  position: absolute;
  top: 18px;
  left: -28px;
  width: 10px;
  height: 10px;
  border: 2px solid var(--bg);
  border-radius: 50%;
  background: var(--primary);
}

.visit h4 {
  display: flex;
  flex-wrap: wrap;
  justify-content: space-between;
  gap: 8px;
  margin: 0 0 8px;
  color: var(--navy);
}

.chip {
  display: inline-block;
  margin: 2px 4px 2px 0;
  padding: 1px 8px;
  border: 1px solid #c9d9e6;
  border-radius: 3px;
  background: var(--primary-soft);
  color: #1f4e79;
  font-size: 12px;
}

.pinfo {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(190px, 1fr));
  gap: 8px 18px;
}

.pinfo small {
  display: block;
  color: var(--muted);
}

/* ===== Visit form checkboxes ===== */
.checks {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 6px;
}

.checks label {
  display: flex;
  align-items: center;
  gap: 8px;
  margin: 0;
  padding: 7px 9px;
  border: 1px solid var(--border);
  border-radius: 3px;
  font-weight: 400;
  color: var(--text);
  cursor: pointer;
}

.checks input {
  width: auto;
}

/* ===== Bill ===== */
.bill {
  padding: 16px;
  border: 1px solid #c5ced6;
  border-radius: 3px;
  background: #fcfcfb;
  font-family: Consolas, "Courier New", monospace;
}

.bill .r   { display: flex; justify-content: space-between; padding: 2px 0; }
.bill .tot { font-size: 15px; font-weight: 700; }
.bill hr   { margin: 8px 0; border: 0; border-top: 1px dashed #aab6c0; }

/* ===== Responsive & print ===== */
@media (max-width: 800px) {
  aside { width: 180px; }
  main  { padding: 16px; }
  .two  { grid-template-columns: 1fr; }
}

@media print {
  .bar, aside, .top, .row, #toast { display: none !important; }
  main { padding: 0; }
  .ov  { position: static; background: none; }
  .md  { border: 0; }
}

/* ===== Report sections ===== */
.report h3 {
  margin: 28px 0 10px;
  padding-bottom: 6px;
  border-bottom: 1px solid var(--border);
  font-size: 16px;
  color: var(--navy);
}

.report h3:first-child {
  margin-top: 0;
}
</style>
</head>
<body>

<div id="login">
  <form class="lcard" id="lf">
    <div class="lhead"><svg width="28" height="28" viewBox="0 0 32 32" aria-hidden="true"><rect width="32" height="32" rx="5" fill="#ffffff"/><path d="M13 7h6v6h6v6h-6v6h-6v-6H7v-6h6z" fill="#2b6a99"/></svg><div><b>Hospital Management System</b><small>Staff login</small></div></div>
    <div class="lbody">
      <label>Username</label><input id="lu" autocomplete="username" autofocus>
      <label>Password</label><input id="lp" type="password" autocomplete="current-password">
      <div class="err" id="le"></div>
      <button class="btn full">Login</button>
      <div class="hint"><b>Demo accounts</b><br>Admin: admin01 / admin123<br>Reception: recep01 / recep123<br>Doctor: dr.sharma / doc123</div>
    </div>
  </form>
</div>

<div id="app" class="hide">
  <header class="bar">
    <div class="bar-brand"><svg width="28" height="28" viewBox="0 0 32 32" aria-hidden="true"><rect width="32" height="32" rx="5" fill="#ffffff"/><path d="M13 7h6v6h6v6h-6v6h-6v-6H7v-6h6z" fill="#2b6a99"/></svg>Hospital Management System</div>
    <div class="bar-user"><b id="uname"></b><span id="urole"></span></div>
  </header>
  <div class="shell">
    <aside>
      <nav id="nav"></nav>
      <button class="lo" onclick="logout()"><svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"><path d="M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4"/><polyline points="16 17 21 12 16 7"/><line x1="21" y1="12" x2="9" y2="12"/></svg>Logout</button>
    </aside>
    <main>
      <div class="top"><h2 id="ptitle"></h2><div class="tools" id="ptools"></div></div>
      <div id="content"></div>
    </main>
  </div>
</div>
<div id="toast" class="hide"></div>
<script>
/* core.js - shared helpers, API call, login, menu, start-up
   Owner: Nabin
   Used by every page file. Load this file first. */

/* ========== 1. HELPERS ========== */

const $ = selector => document.querySelector(selector);

// Escape text so user input can't break the page
function esc(text) {
  const map = { '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' };
  return String(text ?? '').replace(/[&<>"']/g, ch => map[ch]);
}

function money(amount) {
  return 'Rs. ' + Number(amount).toLocaleString('en-IN');
}

function setContent(html) {
  $('#content').innerHTML = html;
}

function emptyBox(message) {
  return `<div class="card empty">${message}</div>`;
}

function tag(text) {
  return `<span class="tag ${text}">${text}</span>`;
}

function stat(number, label) {
  return `<div class="stat"><div class="n">${number}</div><div class="l">${label}</div></div>`;
}

// Small button used inside tables
function smallButton(label, onclick, style = '') {
  return `<button class="btn sm ${style}" onclick="${onclick}">${label}</button>`;
}

// <option> list. items = array, getValue/getLabel = functions
function options(items, getValue, getLabel, selected = '') {
  return items.map(item => {
    const value = getValue(item);
    return `<option value="${value}" ${value === selected ? 'selected' : ''}>${getLabel(item)}</option>`;
  }).join('');
}

// Build a table. headers = array of text, rows = array of arrays of cell HTML
function table(headers, rows) {
  const head = headers.map(h => `<th>${h}</th>`).join('');
  const body = rows.map(cells => '<tr>' + cells.map(c => `<td>${c}</td>`).join('') + '</tr>').join('');
  return `<table><tr>${head}</tr>${body}</table>`;
}

function toast(message, isError) {
  const box = $('#toast');
  box.textContent = message;
  box.className = isError ? 'bad' : '';
  clearTimeout(toast.timer);
  toast.timer = setTimeout(() => box.className = 'hide', 3000);
}

// Popup form. onSubmit(formData) runs on Save; pass null for an info-only popup.
function showModal(title, body, onSubmit, saveText = 'Save') {
  const overlay = document.createElement('div');
  overlay.className = 'ov';
  overlay.innerHTML = `
    <form class="md">
      <h3>${esc(title)}</h3>
      ${body}
      <div class="err"></div>
      <div class="row">
        <button type="button" class="btn ghost" data-x>${onSubmit ? 'Cancel' : 'Close'}</button>
        ${onSubmit ? `<button class="btn">${saveText}</button>` : ''}
      </div>
    </form>`;
  document.body.appendChild(overlay);

  const close = () => overlay.remove();
  overlay.querySelector('[data-x]').onclick = close;
  overlay.onmousedown = e => { if (e.target === overlay) close(); };

  overlay.querySelector('form').onsubmit = async e => {
    e.preventDefault();
    if (!onSubmit) return close();
    try {
      await onSubmit(Object.fromEntries(new FormData(e.target)));
      close();
    } catch (error) {
      overlay.querySelector('.err').textContent = error.message;
    }
  };
  return overlay;
}


/* ========== 2. STATE & API ========== */

let token = localStorage.getItem('hms_token');
let me = null;                  // logged-in user {id, name, role, pages, fee}
let historyPatientId = '';      // patient shown on the History page

const can = (...roles) => roles.includes(me.role);

// Talk to the C++ server. Pass `data` to send a POST.
async function api(path, data) {
  const request = { headers: { 'X-Token': token || '' } };
  if (data) {
    request.method = 'POST';
    request.headers['Content-Type'] = 'application/x-www-form-urlencoded';
    request.body = new URLSearchParams(data).toString();
  }
  const response = await fetch('/api/' + path, request);
  const result = await response.json().catch(() => ({ error: 'Server error' }));

  if (response.status === 401 && path !== 'login') {
    showLogin();
    throw new Error(result.error);
  }
  if (!response.ok) throw new Error(result.error || 'Error');
  return result;
}


/* ========== 3. LOGIN / SHELL ========== */

// Simple line icons for the menu (drawn as SVG, no image files)
const icon = shapes => '<svg class="ic" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" '
                     + 'stroke-linecap="round" stroke-linejoin="round">' + shapes + '</svg>';

const ICONS = {
  dashboard:     icon('<rect x="3" y="3" width="7" height="7"/><rect x="14" y="3" width="7" height="7"/><rect x="14" y="14" width="7" height="7"/><rect x="3" y="14" width="7" height="7"/>'),
  patients:      icon('<path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"/><circle cx="12" cy="7" r="4"/>'),
  doctors:       icon('<polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/>'),
  receptionists: icon('<path d="M16 4h2a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2"/><rect x="8" y="2" width="8" height="4" rx="1"/>'),
  appointments:  icon('<rect x="3" y="4" width="18" height="18" rx="2"/><line x1="16" y1="2" x2="16" y2="6"/><line x1="8" y1="2" x2="8" y2="6"/><line x1="3" y1="10" x2="21" y2="10"/>'),
  history:       icon('<circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/>'),
  visit:         icon('<rect x="3" y="3" width="18" height="18" rx="2"/><line x1="12" y1="8" x2="12" y2="16"/><line x1="8" y1="12" x2="16" y2="12"/>'),
  bills:         icon('<rect x="1" y="4" width="22" height="16" rx="2"/><line x1="1" y1="10" x2="23" y2="10"/>'),
  reports:       icon('<line x1="18" y1="20" x2="18" y2="10"/><line x1="12" y1="20" x2="12" y2="4"/><line x1="6" y1="20" x2="6" y2="14"/>')
};

// page id -> function that draws it (each page lives in its own file)
const PAGES = {
  dashboard: () => showDashboard(),
  patients: () => showPatients(),
  appointments: () => showAppointments(),
  history: () => showHistory(),
  visit: () => showVisitForm(),
  bills: () => showBills(),
  reports: () => showReports(),
  doctors: () => showStaff('Doctor'),
  receptionists: () => showStaff('Receptionist')
};

$('#lf').onsubmit = async e => {
  e.preventDefault();
  try {
    me = await api('login', { username: $('#lu').value, password: $('#lp').value });
    token = me.token;
    localStorage.setItem('hms_token', token);
    showApp();
  } catch (error) {
    $('#le').textContent = error.message;
  }
};

function showLogin() {
  token = null;
  me = null;
  localStorage.removeItem('hms_token');
  $('#app').classList.add('hide');
  $('#login').classList.remove('hide');
  $('#lp').value = '';
  $('#le').textContent = '';
}

async function logout() {
  try { await api('logout', {}); } catch (error) { /* ignore */ }
  showLogin();
}

function showApp() {
  $('#login').classList.add('hide');
  $('#app').classList.remove('hide');
  $('#uname').textContent = me.name;
  $('#urole').textContent = me.role;

  // Sidebar menu comes from the server, so each role sees its own pages
  $('#nav').innerHTML = me.pages.map(p =>
    `<button data-p="${p.id}" onclick="go('${p.id}')">${ICONS[p.id] || ''}<span>${esc(p.label)}</span></button>`
  ).join('');
  go('dashboard');
}

// Open a page. `arg` is only used to open History for a chosen patient.
function go(pageId, arg) {
  if (pageId === 'history' && arg !== undefined) historyPatientId = arg;

  document.querySelectorAll('#nav button').forEach(button =>
    button.classList.toggle('on', button.dataset.p === pageId));

  $('#ptools').innerHTML = '';
  const menuItem = me.pages.find(p => p.id === pageId);   // History has no menu item for Admin
  $('#ptitle').textContent = menuItem ? menuItem.label : 'Patient History';

  PAGES[pageId]().catch(error => setContent(emptyBox(esc(error.message))));
}


/* ========== 12. START ========== */

// If a login token is saved, go straight to the app
(async () => {
  if (!token) return;
  try {
    me = await api('me');
    showApp();
  } catch (error) { /* token expired: stay on login */ }
})();
</script>
<script>
/* dashboard.js
   Owner: Sarthak
   Dashboard page. */

/* ========== 4. DASHBOARD ========== */

async function showDashboard() {
  const s = await api('summary');
  const isDoctor = me.role === 'Doctor';

  let cards = stat(s.patients, 'Patients')
            + stat(s.scheduled, isDoctor ? 'My scheduled appointments' : 'Scheduled appointments')
            + stat(s.today, 'Appointments today');
  if (!isDoctor) cards += stat(s.unpaid, 'Unpaid bills');
  if (me.role === 'Admin') cards += stat(s.doctors, 'Doctors');
  cards += stat(s.visits, 'Medical visits');

  const date = new Date().toLocaleDateString('en-GB', { weekday: 'long', day: 'numeric', month: 'long', year: 'numeric' });
  setContent(`
    <p class="lead">Welcome, <b>${esc(me.name)}</b> &middot; ${date}</p>
    <div class="cards">${cards}</div>`);
}
</script>
<script>
/* patients.js
   Owner: Sushant
   Patients page: list, search, register, edit, delete. */

let patients = [];   // cached list for this page

/* ========== 5. PATIENTS ========== */

const GENDERS = ['Male', 'Female', 'Other'];
const BLOOD_GROUPS = ['A+', 'A-', 'B+', 'B-', 'AB+', 'AB-', 'O+', 'O-'];

// Form fields used by both "Register" and "Edit"
function patientForm(p = {}) {
  return `
    <label>Full name</label>
    <input name="name" value="${esc(p.name)}" required>
    <div class="two">
      <div><label>Date of birth</label><input type="date" name="dob" value="${esc(p.dob)}" required></div>
      <div><label>Gender</label><select name="gender">${options(GENDERS, g => g, g => g, p.gender)}</select></div>
    </div>
    <div class="two">
      <div><label>Phone</label><input name="phone" value="${esc(p.phone)}" required></div>
      <div><label>Blood group</label><select name="blood">${options(BLOOD_GROUPS, b => b, b => b, p.blood)}</select></div>
    </div>
    <label>Address</label>
    <input name="address" value="${esc(p.address)}" required>
    <label>Emergency contact</label>
    <input name="emergency" value="${esc(p.emergency)}" required>`;
}

async function showPatients() {
  const search = `<input id="q" placeholder="Search ID / name / phone…" oninput="loadPatients()">`;
  const addButton = can('Admin', 'Receptionist')
    ? `<button class="btn" onclick="addPatient()">+ Register Patient</button>` : '';
  $('#ptools').innerHTML = search + addButton;
  await loadPatients();
}

async function loadPatients() {
  patients = await api('patients?q=' + encodeURIComponent($('#q').value));
  if (!patients.length) return setContent(emptyBox('No patients found.'));

  const rows = patients.map(p => {
    let actions = smallButton('View', `viewPatient('${p.id}')`, 'ghost')
                + smallButton('History', `go('history','${p.id}')`, 'ghost');
    if (can('Admin', 'Receptionist')) actions += smallButton('Edit', `editPatient('${p.id}')`);
    if (can('Admin')) actions += smallButton('Delete', `deletePatient('${p.id}')`, 'red');
    return [p.id, esc(p.name), p.age, esc(p.gender), esc(p.phone), esc(p.blood), `<div class="act">${actions}</div>`];
  });
  setContent(table(['ID', 'Name', 'Age', 'Gender', 'Phone', 'Blood', 'Actions'], rows));
}

function viewPatient(id) {
  const p = patients.find(x => x.id === id);
  const details = [
    ['Name', p.name],
    ['Date of birth', `${p.dob} (age ${p.age})`],
    ['Gender', p.gender],
    ['Phone', p.phone],
    ['Blood group', p.blood],
    ['Address', p.address],
    ['Emergency contact', p.emergency]
  ];
  const html = details.map(([label, value]) => `<div><small>${label}</small>${esc(value)}</div>`).join('');
  showModal('Patient ' + p.id, `<div class="pinfo" style="margin-top:12px">${html}</div>`, null);
}

function addPatient() {
  showModal('Register Patient', patientForm(), async data => {
    const result = await api('patients', data);
    toast('Registered as ' + result.id);
    loadPatients();
  }, 'Register');
}

function editPatient(id) {
  const p = patients.find(x => x.id === id);
  showModal('Edit Patient ' + id, patientForm(p), async data => {
    await api(`patients/${id}/update`, data);
    toast('Patient updated');
    loadPatients();
  });
}

function deletePatient(id) {
  const message = `<p>This permanently deletes <b>${id}</b> and all their appointments, visits and bills.</p>`;
  showModal('Delete patient?', message, async () => {
    await api(`patients/${id}/delete`, {});
    toast('Patient deleted');
    loadPatients();
  }, 'Delete');
}
</script>
<script>
/* appointments.js
   Owner: Nabin
   Appointments page: list, book, cancel. */

let appointmentFilter = 'All';

/* ========== 6. APPOINTMENTS ========== */

async function showAppointments() {
  const statuses = ['All', 'Scheduled', 'Completed', 'Cancelled'];
  let tools = `<select style="width:150px" onchange="appointmentFilter=this.value; showAppointments()">
                 ${options(statuses, s => s, s => s, appointmentFilter)}</select>`;
  const canBook = can('Admin', 'Receptionist');
  if (canBook) tools += `<button class="btn" onclick="newAppointment()">+ New Appointment</button>`;
  $('#ptools').innerHTML = tools;

  let list = await api('appointments');
  if (appointmentFilter !== 'All') list = list.filter(a => a.status === appointmentFilter);
  if (!list.length) return setContent(emptyBox('No appointments.'));
  list.sort((a, b) => (a.date + a.time).localeCompare(b.date + b.time));

  const rows = list.map(a => {
    const row = [a.id, `${a.patientId} · ${esc(a.patientName)}`, esc(a.doctorName), a.date, a.time, esc(a.reason), tag(a.status)];
    if (canBook) row.push(a.status === 'Scheduled' ? smallButton('Cancel', `cancelAppointment('${a.id}')`, 'red') : '');
    return row;
  });
  const headers = ['ID', 'Patient', 'Doctor', 'Date', 'Time', 'Reason', 'Status'];
  if (canBook) headers.push('');
  setContent(table(headers, rows));
}

async function newAppointment() {
  const [patientList, doctors] = await Promise.all([api('patients'), api('doctors')]);
  if (!patientList.length) return toast('Register a patient first', true);

  const patientOptions = options(patientList, p => p.id, p => `${p.id} · ${esc(p.name)}`);
  const doctorOptions = options(doctors, d => d.id, d => `${esc(d.name)} — ${esc(d.specialization)} (${money(d.fee)})`);
  const todayDate = new Date().toISOString().slice(0, 10);

  showModal('New Appointment', `
    <label>Patient</label><select name="patientId">${patientOptions}</select>
    <label>Doctor</label><select name="doctorId">${doctorOptions}</select>
    <div class="two">
      <div><label>Date</label><input type="date" name="date" value="${todayDate}" required></div>
      <div><label>Time</label><input type="time" name="time" value="10:00" required></div>
    </div>
    <label>Reason</label><input name="reason" required>`,
    async data => {
      const result = await api('appointments', data);
      toast('Appointment ' + result.id + ' created');
      showAppointments();
    }, 'Create');
}

function cancelAppointment(id) {
  showModal('Cancel appointment?', `<p>Cancel appointment <b>${id}</b>?</p>`, async () => {
    await api(`appointments/${id}/cancel`, {});
    toast('Cancelled');
    showAppointments();
  }, 'Yes, cancel');
}
</script>
<script>
/* history.js
   Owner: Sushant
   Patient history timeline and Suggest Test / Procedure. */

/* ========== 7. PATIENT HISTORY (timeline) ========== */

async function showHistory() {
  const list = await api('patients');
  $('#ptools').innerHTML = `
    <select style="width:280px" onchange="historyPatientId=this.value; loadHistory()">
      <option value="">— select patient —</option>
      ${options(list, p => p.id, p => `${p.id} · ${esc(p.name)}`, historyPatientId)}
    </select>`;
  await loadHistory();
}

// One visit card in the timeline
function visitCard(visit) {
  const services = visit.services.length
    ? visit.services.map(s => `<span class="chip">${esc(s.name)} · ${money(s.price)}</span>`).join('')
    : '<span style="color:var(--muted)">None</span>';

  // Only the doctor who made the visit can add tests / procedures to it
  const doctorButtons = (me.role === 'Doctor' && visit.doctorId === me.id) ? `
    <div class="act" style="margin-top:12px">
      ${smallButton('+ Suggest Test', `suggest('${visit.id}','Test')`)}
      ${smallButton('+ Suggest Procedure', `suggest('${visit.id}','Procedure')`, 'warn')}
    </div>` : '';

  const label = text => `<small style="color:var(--muted)">${text}</small><br>`;
  return `
    <div class="visit">
      <h4>
        <span>Visit ${visit.id} · ${visit.date}</span>
        <span style="color:var(--muted);font-weight:400">${esc(visit.doctorName)}</span>
      </h4>
      <div>${label('DIAGNOSIS')}${esc(visit.diagnosis)}</div>
      <div style="margin-top:8px">${label('NOTES')}${esc(visit.notes)}</div>
      <div style="margin-top:8px">${label('SERVICES')}${services}</div>
      ${doctorButtons}
    </div>`;
}

async function loadHistory() {
  if (!historyPatientId) {
    return setContent(emptyBox('Select a patient to see their medical history.'));
  }
  const { patient: p, visits } = await api(`patients/${historyPatientId}/history`);

  const header = `
    <div class="card"><div class="pinfo">
      <div><small>Patient</small><b>${p.id} · ${esc(p.name)}</b></div>
      <div><small>Age / Gender</small>${p.age} / ${esc(p.gender)}</div>
      <div><small>Blood group</small>${esc(p.blood)}</div>
      <div><small>Phone</small>${esc(p.phone)}</div>
    </div></div>`;

  const timeline = visits.length
    ? `<div class="tl">${visits.map(visitCard).join('')}</div>`
    : emptyBox('No visits recorded yet.');
  setContent(header + timeline);
}

// Doctor adds a test or procedure to an existing visit (price is added to the bill)
async function suggest(visitId, type) {
  const services = (await api('services')).filter(s => s.type === type);
  const serviceOptions = options(services, s => s.id, s => `${esc(s.name)} — ${money(s.price)}`);

  showModal('Suggest ' + type, `
    <label>${type}</label>
    <select name="serviceId">${serviceOptions}</select>
    <p style="color:var(--muted);font-size:13px">The price is added to the patient's bill automatically.</p>`,
    async data => {
      const bill = await api(`visits/${visitId}/services`, data);
      toast(`Added to bill ${bill.id} · total ${money(bill.total)}`);
      loadHistory();
    }, 'Add to bill');
}
</script>
<script>
/* visit.js
   Owner: Sushant
   Add Medical Visit form (doctor). Uses billHtml() from bills.js (same owner). */

/* ========== 8. ADD MEDICAL VISIT (doctor) ========== */

async function showVisitForm() {
  const [patientList, services, appointments] = await Promise.all([api('patients'), api('services'), api('appointments')]);
  const scheduled = appointments.filter(a => a.status === 'Scheduled');

  // Checkbox list for one service type
  const checkboxes = type => services.filter(s => s.type === type).map(s => `
    <label>
      <input type="checkbox" class="sv" value="${s.id}" data-price="${s.price}">
      ${esc(s.name)} <span style="margin-left:auto;color:var(--muted)">${money(s.price)}</span>
    </label>`).join('');

  setContent(`
    <form class="card" id="vf" style="max-width:760px">
      <div class="two">
        <div>
          <label>Patient</label>
          <select name="patientId" id="vp" required>
            <option value="">— select —</option>
            ${options(patientList, p => p.id, p => `${p.id} · ${esc(p.name)}`)}
          </select>
        </div>
        <div>
          <label>Linked appointment (optional)</label>
          <select name="appointmentId" id="va"><option value="">None</option></select>
        </div>
      </div>
      <label>Diagnosis</label><input name="diagnosis" required>
      <label>Medical notes</label><textarea name="notes" required></textarea>
      <label>Tests</label><div class="checks">${checkboxes('Test')}</div>
      <label>Procedures</label><div class="checks">${checkboxes('Procedure')}</div>
      <div class="card" style="background:#f6f8fa;margin:18px 0 0;padding:10px 14px">
        Consultation ${money(me.fee)} + services <b id="vs"></b> = estimated total <b id="vt"></b>
      </div>
      <div class="err" id="ve"></div>
      <button class="btn full">Save visit &amp; generate bill</button>
    </form>`);

  const ticked = () => [...document.querySelectorAll('.sv:checked')];

  // Live total while ticking boxes
  function updateTotal() {
    const servicesTotal = ticked().reduce((sum, box) => sum + Number(box.dataset.price), 0);
    $('#vs').textContent = money(servicesTotal);
    $('#vt').textContent = money(servicesTotal + me.fee);
  }
  document.querySelectorAll('.sv').forEach(box => box.onchange = updateTotal);
  updateTotal();

  // Appointment list depends on the chosen patient
  $('#vp').onchange = () => {
    const mine = scheduled.filter(a => a.patientId === $('#vp').value);
    $('#va').innerHTML = '<option value="">None</option>'
      + options(mine, a => a.id, a => `${a.id} · ${a.date} ${a.time} · ${esc(a.reason)}`);
  };

  $('#vf').onsubmit = async e => {
    e.preventDefault();
    const data = Object.fromEntries(new FormData(e.target));
    data.services = ticked().map(box => box.value).join(',');
    try {
      const result = await api('visits', data);
      showModal('Visit ' + result.visitId + ' saved', billHtml(result.bill), null);
      toast('Visit saved');
      showVisitForm();
    } catch (error) {
      $('#ve').textContent = error.message;
    }
  };
}
</script>
<script>
/* bills.js
   Owner: Sushant
   Bills page and the bill pop-up (billHtml is also used by visit.js (same owner)). */

let bills = [];   // cached list for this page

/* ========== 9. BILLS ========== */

function billHtml(bill) {
  const line = (name, amount) => `<div class="r"><span>${esc(name)}</span><span>${money(amount)}</span></div>`;
  const items = bill.items.map(i => line(i.name, i.price)).join('');
  const other = bill.other ? line('Other charges', bill.other) : '';
  const status = bill.paid ? 'Paid' : 'Unpaid';

  return `
    <div class="bill" style="margin-top:12px">
      <div style="text-align:center;font-weight:700">BILL ${bill.id}</div>
      <div style="text-align:center;color:var(--muted)">${bill.patientId} · ${esc(bill.patientName)} · ${bill.date}</div>
      <hr>
      ${line('Consultation', bill.consultation)}
      ${items}
      ${other}
      <hr>
      <div class="r tot"><span>TOTAL</span><span>${money(bill.total)}</span></div>
      <div class="r"><span>Payment status</span>${tag(status)}</div>
    </div>`;
}

async function showBills() {
  bills = await api('bills');
  if (!bills.length) {
    return setContent(emptyBox('No bills yet. Bills are created when a doctor adds a medical visit.'));
  }
  const rows = bills.map(b => [
    b.id,
    `${b.patientId} · ${esc(b.patientName)}`,
    b.date,
    money(b.total),
    tag(b.paid ? 'Paid' : 'Unpaid'),
    smallButton('Open', `openBill('${b.id}')`)
  ]);
  setContent(table(['Bill', 'Patient', 'Date', 'Total', 'Status', ''], rows));
}

function openBill(id) {
  const bill = bills.find(b => b.id === id);
  const popup = showModal('Bill ' + id, billHtml(bill), null);

  // Extra buttons next to "Close"
  popup.querySelector('.row').insertAdjacentHTML('afterbegin', `
    <button type="button" class="btn ghost" data-print>Print</button>
    ${bill.paid ? '' : '<button type="button" class="btn warn" data-charge>Add charge</button>'}
    <button type="button" class="btn" data-pay>${bill.paid ? 'Mark unpaid' : 'Mark paid'}</button>`);

  popup.querySelector('[data-print]').onclick = () => window.print();

  popup.querySelector('[data-pay]').onclick = async () => {
    await api(`bills/${id}/paid`, { paid: bill.paid ? '0' : '1' });
    popup.remove();
    toast('Payment status updated');
    showBills();
  };

  const chargeButton = popup.querySelector('[data-charge]');
  if (chargeButton) chargeButton.onclick = () => {
    popup.remove();
    showModal('Add other charges', '<label>Amount (Rs.)</label><input type="number" name="amount" min="1" required>',
      async data => {
        await api(`bills/${id}/other`, data);
        toast('Charge added');
        showBills();
      }, 'Add');
  };
}
</script>
<script>
/* reports.js
   Owner: Sushant
   Reports page (Admin): patient, doctor, appointment and billing reports. */

async function showReports() {
  const [r, appointments, bills] = await Promise.all([api('reports'), api('appointments'), api('bills')]);
  $('#ptools').innerHTML = '<button class="btn ghost" onclick="window.print()">Print</button>';

  // 1. Patient report
  const patientRows = r.patients.map(p => [p.id, esc(p.name), p.visits]);
  const patientReport = `
    <h3>1. Patient report</h3>
    <p class="lead">Total patients: <b>${r.patients.length}</b></p>
    ${patientRows.length ? table(['ID', 'Patient', 'Visits'], patientRows) : emptyBox('No patients.')}`;

  // 2. Doctor report
  const doctorRows = r.doctors.map(d => [d.id, esc(d.name), esc(d.specialization), d.appointments, d.visits]);
  const doctorReport = `
    <h3>2. Doctor report</h3>
    ${table(['ID', 'Doctor', 'Specialization', 'Appointments', 'Visits'], doctorRows)}`;

  // 3. Appointment report
  const appointmentRows = appointments.map(a =>
    [a.id, `${a.patientId} · ${esc(a.patientName)}`, esc(a.doctorName), a.date, a.time, tag(a.status)]);
  const appointmentReport = `
    <h3>3. Appointment report</h3>
    <div class="cards">
      ${stat(r.appointments.scheduled, 'Scheduled')}
      ${stat(r.appointments.completed, 'Completed')}
      ${stat(r.appointments.cancelled, 'Cancelled')}
    </div>
    ${appointmentRows.length ? table(['ID', 'Patient', 'Doctor', 'Date', 'Time', 'Status'], appointmentRows) : emptyBox('No appointments.')}`;

  // 4. Billing report (the only place with money)
  const billRows = bills.map(b =>
    [b.id, `${b.patientId} · ${esc(b.patientName)}`, b.date, money(b.total), tag(b.paid ? 'Paid' : 'Unpaid')]);
  const billingReport = `
    <h3>4. Billing report</h3>
    <div class="cards">
      ${stat(r.billing.count, 'Bills')}
      ${stat(money(r.billing.paid), 'Collected (paid)')}
      ${stat(money(r.billing.unpaid), 'Outstanding (unpaid)')}
      ${stat(money(r.billing.total), 'Total billed')}
    </div>
    ${billRows.length ? table(['Bill', 'Patient', 'Date', 'Total', 'Status'], billRows) : emptyBox('No bills yet.')}`;

  setContent(`<div class="report">${patientReport}${doctorReport}${appointmentReport}${billingReport}</div>`);
}
</script>
<script>
/* staff.js
   Owner: Sushant
   Doctors and Receptionists pages (Admin). */

/* ========== 11. STAFF (admin: doctors & receptionists) ========== */

async function showStaff(role) {
  $('#ptools').innerHTML = `<button class="btn" onclick="addStaff('${role}')">+ Add ${role}</button>`;
  const list = await api('staff/' + role);
  if (!list.length) return setContent(emptyBox(`No ${role.toLowerCase()}s yet.`));

  const isDoctor = role === 'Doctor';
  const rows = list.map(u => {
    const actions = smallButton('Reset password', `resetPassword('${u.id}')`, 'ghost')
                  + smallButton('Delete', `deleteStaff('${u.id}','${role}')`, 'red');
    const row = [u.id, esc(u.name), esc(u.username)];
    if (isDoctor) row.push(esc(u.specialization), money(u.fee));
    row.push(`<div class="act">${actions}</div>`);
    return row;
  });
  const headers = ['ID', 'Name', 'Username'];
  if (isDoctor) headers.push('Specialization', 'Fee');
  headers.push('');
  setContent(table(headers, rows));
}

function addStaff(role) {
  const doctorFields = `
    <div class="two">
      <div><label>Specialization</label><input name="specialization" required></div>
      <div><label>Consultation fee (Rs.)</label><input type="number" name="fee" min="1" required></div>
    </div>`;

  showModal('Add ' + role, `
    <input type="hidden" name="role" value="${role}">
    <label>Full name</label><input name="name" required>
    <div class="two">
      <div><label>Username</label><input name="username" required></div>
      <div><label>Password</label><input name="password" required></div>
    </div>
    ${role === 'Doctor' ? doctorFields : ''}`,
    async data => {
      await api('staff', data);
      toast(role + ' added');
      showStaff(role);
    }, 'Add');
}

function resetPassword(id) {
  showModal('Reset password for ' + id, '<label>New password</label><input name="password" required>', async data => {
    await api(`staff/${id}/password`, data);
    toast('Password updated');
  });
}

function deleteStaff(id, role) {
  const message = `<p>Delete <b>${id}</b>? This is blocked if they have appointment or visit records.</p>`;
  showModal('Delete ' + role + '?', message, async () => {
    await api(`staff/${id}/delete`, {});
    toast('Deleted');
    showStaff(role);
  }, 'Delete');
}
</script>
</body>
</html>
)HMSPAGE";
