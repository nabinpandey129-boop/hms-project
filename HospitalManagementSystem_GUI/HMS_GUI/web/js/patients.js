/* patients.js

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
