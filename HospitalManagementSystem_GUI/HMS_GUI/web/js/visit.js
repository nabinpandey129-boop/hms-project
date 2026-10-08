/* visit.js

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
