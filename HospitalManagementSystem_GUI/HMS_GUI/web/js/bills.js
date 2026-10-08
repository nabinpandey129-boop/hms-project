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
