/* core.js - shared helpers, API call, login, menu, start-up
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
