/* staff.js
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
