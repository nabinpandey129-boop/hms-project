/* reports.js
  
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
