/* dashboard.js

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
