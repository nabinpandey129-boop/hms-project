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
