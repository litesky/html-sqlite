var sqlite = {
		name: null, // Table Name // Need
		struc: null, // Table Struc
		text: null, // Response Text
		db: null, // Table Data Object
		sole: null, // Table Sole Key Name //Need
		index: null, // Table Index Key Name //Need, Some Way Index == Sole
		limit: null, // Select Limit // Need
		state: true, // This State
		offset: 0, // Offset Of Limit
		type: 0 // Is From Select
	}
	//

sqlite.req_bas = function(url, sData) {
		var nBytes = sData.length;
		var nIcx = 0;
		for (var nIdx = 0; nIdx < nBytes; nIdx++) {
			var sChar = sData.charCodeAt(nIdx);
			if (sChar > 65535) {
				nIcx = nIcx + 3;
			} else {
				nIcx = nIcx + 2;
			}
		}
		ui8Data = new Uint8Array(nIcx + 2);
		nIcx = 0;
		for (var nIdx = 0; nIdx < nBytes; nIdx++) {
			var sChar = sData.charCodeAt(nIdx);
			if (sChar > 65535) {
				ui8Data[nIcx] = sChar & 255;
				nIcx++;
				ui8Data[nIcx] = (sChar & 65280) / 256;
				nIcx++;
				ui8Data[nIcx] = (sChar & 16711680) / 65536;
				nIcx++;
			} else {
				ui8Data[nIcx] = sChar & 255;
				nIcx++;
				ui8Data[nIcx] = (sChar & 65280) / 256;
				nIcx++;
			}
		}
		ui8Data[nIcx++] = 0;
		ui8Data[nIcx] = 0;
		//
		var xhr = new XMLHttpRequest();
		xhr.open('POST', url, false);
		xhr.onreadystatechange = function() {
			if (xhr.readyState == 4) {
				return xhr.responseText;
			}
		}
		xhr.send(ui8Data);
		return xhr.onreadystatechange();
	}
	//

sqlite.req_pro = function(url, dat) {
		var txt = this.req_bas(url, dat);
		this.text = txt;
		var obj = JSON.parse(txt);
		document.title = obj['code'];
		if (this.type == 0) {

			this.db = obj['db'];
		}
	}
	//

sqlite.escstr = function(src) {
		src = src.replace(/[\/]/g, '//'); // / 优先 / FIrst
		src = src.replace(/]/g, '/]');
		src = src.replace(/[%]/g, '/%');
		src = src.replace(/[&]/g, '/&');
		src = src.replace(/[_]/g, '/_');
		src = src.replace(/[\[]/g, '/[');
		src = src.replace(/[\(]/g, '/(');
		src = src.replace(/[\)]/g, '/)');
		return src;
	}
	//

sqlite.term = function(key, value, s) { //条件
		var str;
		if (value) {
			value = value.replace(/'/g, '\'\'');
		}
		if ((this.struc[key] & 1) == 1) {
			if (s) {
				str = '\'' + this.escstr(value) + '\'';
			} else {
				str = '\'' + value + '\'';
			}
		} else if ((this.struc[key] & 2) == 2) {
			str = value;
		} else {
			str = '\'Lost Key\'';
			this.req_bas = function(x, y) { // Trap
				alert(y)
			};
		}
		return str;
	}
	//

sqlite.arch = function() {
		var rule = {};
		var str = this.text
		str = str.replace(/[\[\r\n\]]/g, '');
		str = str.replace(/\\r/g, '')
		str = str.replace(/\\n/g, '')
		str = str.match(/\((.)+\)/)[0];
		str = str.substr(1, str.length - 2);
		var arr = str.split(/, /);
		for (var i = 0; i < arr.length; i++) {
			str = arr[i];
			var key = str.match(/[\S]+/);
			var val = str.match(/ TEXT/);
			rule[key] = 2;
			if (val) {
				rule[key] = 1;
			}
		}
		this.struc = rule;
	}
	//

sqlite.select = function(start, isAsc) {
		var sql_srt = [
			'select * from',
			this.name,
			'where'
		];
		var index = this.index;
		var size = this.db.length;
		// var size = sql_srt.length;
		var cnd = true;
		if (start) {
			sql_srt.push(index);
			if (isAsc) {
				sql_srt.push('>');
			} else {
				sql_srt.push('<');
			}
			sql_srt.push(start);
			cnd = null;
			this.state = null;
		}
		if (cnd && (this.state)) { // Run Once
			sql_srt.push(index);
			sql_srt.push('>0');
			cnd = null;
			this.state = null;
		}
		if (cnd && (size == (this.limit + this.offset))) { // Is Not End?
			sql_srt.push(index);
			if (isAsc) {
				sql_srt.push('>');
			} else {
				sql_srt.push('<');
			}
			sql_srt.push(this.db[size - 1][index]);
			cnd = null;
			this.state = null;
		}
		if (cnd) {
			return false;
		}
		if (index) {
			sql_srt.push('order by');
			sql_srt.push(index);
			if (isAsc) {
				sql_srt.push('asc');
			} else {
				sql_srt.push('desc');
			}
		}
		if (this.limit) {
			sql_srt.push('limit');
			sql_srt.push(this.limit);
		}
		sql_srt.push(';');
		this.type = 0;
		return sql_srt.join(' ');
	}
	//

sqlite.replace = function(arr) {
		var sql_srt = [
			'replace into ',
			this.name,
			'(',
			null,
			') values (',
			null,
			');'
		];
		var keys = [];
		var values = [];
		for (var i = 0; i < arr.length; i = i + 2) {
			keys.push(arr[i]);
			values.push(this.term(arr[i], arr[i + 1]));
		}
		sql_srt[3] = (keys.join(','));
		sql_srt[5] = (values.join(','));
		this.type = 15;
		return sql_srt.join(' ');
	}
	//

sqlite.delete = function(arr) {
	var sql_srt = [
		'delete from',
		this.name,
		'where',
		null,
		';'
	];
	for (var i = 0; i < arr.length; i = i + 2) {
		if (this.sole == arr[i]) {
			sql_srt[3] = (this.sole + '=' + this.term(arr[i], arr[i + 1]));
			return sql_srt.join(' ');
			break;
		}
	}
	this.type = 15;
	return '';
}
sqlite.match = function(arr, bar, inf) {
	var sql_srt = [
		'select * from',
		this.name,
		'where'
	];
	var size = arr.length;
	if (sqlite.struc[arr[0]] == 2) {
		sql_srt.push(arr[0]);
		sql_srt.push(bar);
		sql_srt.push(inf);
	} else {
		for (var xx = 0; xx < size; xx++) {
			sql_srt.push(arr[xx]);
			sql_srt.push(bar);
			sql_srt.push(this.term(arr[xx], inf, 1));
			if (xx + 1 < size) {
				sql_srt.push('OR');
			};
		};
	}
	sql_srt.push(';');
	this.type = 0;
	return sql_srt.join(' ');
}