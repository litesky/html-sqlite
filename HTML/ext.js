sqlite.name = 'Nov';
sqlite.sole = 'Book';
sqlite.index = 'Date';
sqlite.limit = 30;
sqlite.offset = 1;
//
function ext_req(xx) {
  sqlite.req_pro('http://localhost:9050/', xx);
}
//

function ext_init() {
  ext_req('select * from sqlite_master where type="table" and name=\'' + sqlite.name + '\';');
  sqlite.arch();
  ext_dir();
}
//

function ext_table(start, xx) {
  if (sqlite.struc == null) {
    ext_init();
  }
  if (null) {
    start = (new Date()).toString().match(/\d+ \d+:[:\d]+/);
    start = start[0].replace(/[ :]/g, '') //lack of M D
  }
  var AAA = sqlite.select(start, xx);
  if (AAA) {
    ext_req(AAA);
    return false;
  }
  return true;
}
//

function ext_web(xxx) {
  if (xxx) {
    return false;
  }
  if (sqlite.type != 0) {
	  return false;
  }
  var curRow = 1;
  var csvAAA = document.getElementById('AAA');
  var csv = sqlite.db;
  var xxx = null;
  while (csv[curRow]) {
    var date = csv[curRow]['Date'];
    var time = parseInt(date);
    time = new Date(time * 1000);
    time = time.toLocaleFormat('%Y-%m-%d %H:%M:%S');
    var xxx = encodeURIComponent(csv[curRow][sqlite.sole]).replace(/%/g, '');
    var li = document.createElement('li');
    //
    li.setAttribute('name', xxx);
    var div = document.createElement('div');
    div.className = 'timeline-time';
    var h4 = document.createElement('a');
    var nodeText = document.createTextNode(time);
    h4.setAttribute('Name', 'Time' + xxx);
    h4.appendChild(nodeText);
    div.appendChild(h4);
    div.appendChild(document.createElement('br'));
    div.appendChild(document.createElement('br'));
    var h4 = document.createElement('a');
    var nodeText = document.createTextNode(date);
    h4.setAttribute('Name', 'Date' + xxx);
    h4.appendChild(nodeText);
    div.appendChild(h4);
    li.appendChild(div);
    //
    div = document.createElement('div');
    div.className = 'timeline-time-mod';
    var h4 = document.createElement('h4');
    var nodeText = document.createTextNode(csv[curRow]['Rate']);
    h4.setAttribute('Name', 'Rate' + xxx);
    h4.appendChild(nodeText);
    div.appendChild(h4);
    //
    var elm = document.createElement('a');
    elm.href = 'javascript: ext_pop_ui(\'' + xxx + '\');';
    elm.appendChild(document.createTextNode('Edit'));
    div.appendChild(elm);
    li.appendChild(div);
    //
    div = document.createElement('div');
    div.className = 'timeline-content';
    var h4 = document.createElement('a');
    var nodeText = document.createTextNode(csv[curRow]['Book'])
    h4.setAttribute('Name', 'Book' + xxx);
    h4.appendChild(nodeText);
    div.appendChild(h4);
    var h4 = document.createElement('a');
    var nodeText = document.createTextNode('----')
    h4.appendChild(nodeText);
    div.appendChild(h4);
    var h4 = document.createElement('a');
    var nodeText = document.createTextNode(csv[curRow]['Writer'])
    h4.setAttribute('Name', 'Writer' + xxx);
    h4.appendChild(nodeText);
    div.appendChild(h4);
    var p = document.createElement('p');
    var nodeText = document.createTextNode(csv[curRow]['Review']);
    p.setAttribute('Name', 'Review' + xxx);
    p.appendChild(nodeText);
    div.appendChild(p);
    li.appendChild(div);
    //
    csvAAA.appendChild(li);
    ++curRow;
  }
}
function ext_add() {
  var arr = [
  ];
  for (var value in sqlite.struc) {
    var obj = document.getElementsByName('pop' + value);
    if (obj[0]) {
      var str = obj[0].value;
      if (str) {
        arr.push(value);
        arr.push(str);
      }
    }
  };
  hide();
  ext_req(sqlite.replace(arr));
};
//
function ext_del() {
  var obj = document.getElementsByName('pop' + sqlite.sole);
  var str = obj[0].value;
  if (str) {
    hide();
    ext_req(sqlite.delete ([sqlite.sole,
    str
    ]));
  }
};
//
function ext_pop_ui(xxx) {
  var csvTab = document.getElementById('csvTab');
  csvTab.innerHTML = '';
  var rowAcc = 0;
  for (var value in sqlite.struc) {
    if (value != null) {
      tr = document.createElement('tr');
      td = document.createElement('td');
      td.appendChild(document.createTextNode(value));
      tr.appendChild(td);
      td = document.createElement('td');
      ti = document.createElement('textarea');
      ti.style.width = '700px';
      ti.style.height = '20px';
      ti.style.textAlign = 'center'; //style.verticalAlign
      if (value == 'Review') {
        ti.style.height = '66px';
      }
      ti.style.resize = 'none';
      var elm = document.getElementsByName(value + xxx)
      if (elm[0]) {
        ti.value = elm[0].textContent;
        //ti.value = elm[0].text;
      } else {
        ti.value = '';
      }
      ti.setAttribute('name', 'pop' + value);
      ti.setAttribute('type', 'text');
      rowAcc++;
      td.appendChild(ti);
      tr.appendChild(td);
      csvTab.appendChild(tr);
    }
  };
  var tr = document.createElement('tr');

  var td = document.createElement('td');
  tr.appendChild(td);

  var td = document.createElement('td');
  var tb = document.createElement('button');
  tb.setAttribute('onclick', 'ext_add();');
  tb.setAttribute('class','btnName');
  tb.appendChild(document.createTextNode('Mod'));
  td.appendChild(tb);
  
  var tb = document.createElement('button');
  tb.setAttribute('onclick', 'ext_del();');
  tb.setAttribute('class','btnName');
  tb.appendChild(document.createTextNode('Del'));
  td.appendChild(tb);
  tr.appendChild(td);
  csvTab.appendChild(tr);
  var o = document.getElementById('pop');
  o.style.display = 'block';
};
//
function hide() {
  var o = document.getElementById('pop');
  o.style.display = 'none';
};

function ext_dir() {
  var acc = 0;
  var box = document.getElementsByName('dir');
  var elm = document.createElement('option');
  elm.setAttribute('value', 0);
  elm.appendChild(document.createTextNode('ALL TEXT'));
  box[0].options.add(elm);
  for (var value in sqlite.struc) {
    var elm = document.createElement('option');
    elm.setAttribute('value', value);
    elm.appendChild(document.createTextNode(value));
    box[0].options.add(elm);
  };
  var arr = ["GLOB","LIKE","="];
  var box = document.getElementsByName('bar');
  box[0].setAttribute('title', null);
  ext_bar();
	return true;
}

function ext_isEnt(evt) {
  var k = window.event ? evt.keyCode : evt.which;
  if (k == 13) {
    ext_select();
  }
}

function ext_select() {
  var box = document.getElementsByName('dir');
  var arr = [
  ];
  var val = box[0].value;
  if (val == '0') {
    for (var key in sqlite.struc) {
      if (sqlite.struc[key] == 1) {
        arr.push(key);
      }
    }
  } else {
    arr.push(val);
  }
  var box = document.getElementsByName('bar');
  var bar = box[0].value;
  var box = document.getElementsByName('inf');
  var inf = box[0].value;
  ext_req(sqlite.match(arr, bar, inf));
  var csvAAA = document.getElementById('AAA');
  csvAAA.innerHTML = '';
  ext_web()
}
function ext_bar() {
 var box = document.getElementsByName('dir');
 var val = box[0].value;
 if (val == '0') {
	 var arr = ["GLOB","LIKE","="];
	 var dst = '1';
	 } else {
		 if (sqlite.struc[val] == 1) {
	 var arr = ["GLOB","LIKE","="];
	 var dst = '1';
		 }
		 if (sqlite.struc[val] == 2) {
	 var arr = [">","=","<"];
	 var dst = '2';
		 }
 };
  var box = document.getElementsByName('bar');
  var title = box[0].title;
  if (dst !=title)
  {
	  for (var ii = 0; box[0].length;ii++) {
		  box[0].remove(0);
	  }
	  box[0].setAttribute('title', dst);
  for (var value in arr) {
    var elm = document.createElement('option');
    elm.setAttribute('value', arr[value]);
    elm.appendChild(document.createTextNode(arr[value]));
    box[0].options.add(elm);
  };
  }
	return true;
}
