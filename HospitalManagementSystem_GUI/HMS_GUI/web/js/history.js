/* history.js
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
