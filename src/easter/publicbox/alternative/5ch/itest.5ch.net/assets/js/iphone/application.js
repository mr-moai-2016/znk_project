(function (e, t) {
  var n, r, i = typeof t,
    o = e.location,
    a = e.document,
    s = a.documentElement,
    l = e.jQuery,
    u = e.$,
    c = {},
    p = [],
    f = "1.10.2",
    d = p.concat,
    h = p.push,
    g = p.slice,
    m = p.indexOf,
    y = c.toString,
    v = c.hasOwnProperty,
    b = f.trim,
    x = function (e, t) {
      return new x.fn.init(e, t, r)
    },
    w = /[+-]?(?:\d*\.|)\d+(?:[eE][+-]?\d+|)/.source,
    T = /\S+/g,
    C = /^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g,
    N = /^(?:\s*(<[\w\W]+>)[^>]*|#([\w-]*))$/,
    k = /^<(\w+)\s*\/?>(?:<\/\1>|)$/,
    E = /^[\],:{}\s]*$/,
    S = /(?:^|:|,)(?:\s*\[)+/g,
    A = /\\(?:["\\\/bfnrt]|u[\da-fA-F]{4})/g,
    j = /"[^"\\\r\n]*"|true|false|null|-?(?:\d+\.|)\d+(?:[eE][+-]?\d+|)/g,
    D = /^-ms-/,
    L = /-([\da-z])/gi,
    H = function (e, t) {
      return t.toUpperCase()
    },
    q = function (e) {
      (a.addEventListener || "load" === e.type || "complete" === a.readyState) && (_(), x.ready())
    },
    _ = function () {
      a.addEventListener ? (a.removeEventListener("DOMContentLoaded", q, !1), e.removeEventListener("load", q, !1)) : (a.detachEvent("onreadystatechange", q), e.detachEvent("onload", q))
    };
  x.fn = x.prototype = {
    jquery: f,
    constructor: x,
    init: function (e, n, r) {
      var i, o;
      if (!e) return this;
      if ("string" == typeof e) {
        if (i = "<" === e.charAt(0) && ">" === e.charAt(e.length - 1) && e.length >= 3 ? [null, e, null] : N.exec(e), !i || !i[1] && n) return !n || n.jquery ? (n || r).find(e) : this.constructor(n).find(e);
        if (i[1]) {
          if (n = n instanceof x ? n[0] : n, x.merge(this, x.parseHTML(i[1], n && n.nodeType ? n.ownerDocument || n : a, !0)), k.test(i[1]) && x.isPlainObject(n))
            for (i in n) x.isFunction(this[i]) ? this[i](n[i]) : this.attr(i, n[i]);
          return this
        }
        if (o = a.getElementById(i[2]), o && o.parentNode) {
          if (o.id !== i[2]) return r.find(e);
          this.length = 1, this[0] = o
        }
        return this.context = a, this.selector = e, this
      }
      return e.nodeType ? (this.context = this[0] = e, this.length = 1, this) : x.isFunction(e) ? r.ready(e) : (e.selector !== t && (this.selector = e.selector, this.context = e.context), x.makeArray(e, this))
    },
    selector: "",
    length: 0,
    toArray: function () {
      return g.call(this)
    },
    get: function (e) {
      return null == e ? this.toArray() : 0 > e ? this[this.length + e] : this[e]
    },
    pushStack: function (e) {
      var t = x.merge(this.constructor(), e);
      return t.prevObject = this, t.context = this.context, t
    },
    each: function (e, t) {
      return x.each(this, e, t)
    },
    ready: function (e) {
      return x.ready.promise().done(e), this
    },
    slice: function () {
      return this.pushStack(g.apply(this, arguments))
    },
    first: function () {
      return this.eq(0)
    },
    last: function () {
      return this.eq(-1)
    },
    eq: function (e) {
      var t = this.length,
        n = +e + (0 > e ? t : 0);
      return this.pushStack(n >= 0 && t > n ? [this[n]] : [])
    },
    map: function (e) {
      return this.pushStack(x.map(this, function (t, n) {
        return e.call(t, n, t)
      }))
    },
    end: function () {
      return this.prevObject || this.constructor(null)
    },
    push: h,
    sort: [].sort,
    splice: [].splice
  }, x.fn.init.prototype = x.fn, x.extend = x.fn.extend = function () {
    var e, n, r, i, o, a, s = arguments[0] || {},
      l = 1,
      u = arguments.length,
      c = !1;
    for ("boolean" == typeof s && (c = s, s = arguments[1] || {}, l = 2), "object" == typeof s || x.isFunction(s) || (s = {}), u === l && (s = this, --l); u > l; l++)
      if (null != (o = arguments[l]))
        for (i in o) e = s[i], r = o[i], s !== r && (c && r && (x.isPlainObject(r) || (n = x.isArray(r))) ? (n ? (n = !1, a = e && x.isArray(e) ? e : []) : a = e && x.isPlainObject(e) ? e : {}, s[i] = x.extend(c, a, r)) : r !== t && (s[i] = r));
    return s
  }, x.extend({
    expando: "jQuery" + (f + Math.random()).replace(/\D/g, ""),
    noConflict: function (t) {
      return e.$ === x && (e.$ = u), t && e.jQuery === x && (e.jQuery = l), x
    },
    isReady: !1,
    readyWait: 1,
    holdReady: function (e) {
      e ? x.readyWait++ : x.ready(!0)
    },
    ready: function (e) {
      if (e === !0 ? !--x.readyWait : !x.isReady) {
        if (!a.body) return setTimeout(x.ready);
        x.isReady = !0, e !== !0 && --x.readyWait > 0 || (n.resolveWith(a, [x]), x.fn.trigger && x(a).trigger("ready").off("ready"))
      }
    },
    isFunction: function (e) {
      return "function" === x.type(e)
    },
    isArray: Array.isArray || function (e) {
      return "array" === x.type(e)
    },
    isWindow: function (e) {
      return null != e && e == e.window
    },
    isNumeric: function (e) {
      return !isNaN(parseFloat(e)) && isFinite(e)
    },
    type: function (e) {
      return null == e ? e + "" : "object" == typeof e || "function" == typeof e ? c[y.call(e)] || "object" : typeof e
    },
    isPlainObject: function (e) {
      var n;
      if (!e || "object" !== x.type(e) || e.nodeType || x.isWindow(e)) return !1;
      try {
        if (e.constructor && !v.call(e, "constructor") && !v.call(e.constructor.prototype, "isPrototypeOf")) return !1
      } catch (r) {
        return !1
      }
      if (x.support.ownLast)
        for (n in e) return v.call(e, n);
      for (n in e);
      return n === t || v.call(e, n)
    },
    isEmptyObject: function (e) {
      var t;
      for (t in e) return !1;
      return !0
    },
    error: function (e) {
      throw Error(e)
    },
    parseHTML: function (e, t, n) {
      if (!e || "string" != typeof e) return null;
      "boolean" == typeof t && (n = t, t = !1), t = t || a;
      var r = k.exec(e),
        i = !n && [];
      return r ? [t.createElement(r[1])] : (r = x.buildFragment([e], t, i), i && x(i).remove(), x.merge([], r.childNodes))
    },
    parseJSON: function (n) {
      return e.JSON && e.JSON.parse ? e.JSON.parse(n) : null === n ? n : "string" == typeof n && (n = x.trim(n), n && E.test(n.replace(A, "@").replace(j, "]").replace(S, ""))) ? Function("return " + n)() : (x.error("Invalid JSON: " + n), t)
    },
    parseXML: function (n) {
      var r, i;
      if (!n || "string" != typeof n) return null;
      try {
        e.DOMParser ? (i = new DOMParser, r = i.parseFromString(n, "text/xml")) : (r = new ActiveXObject("Microsoft.XMLDOM"), r.async = "false", r.loadXML(n))
      } catch (o) {
        r = t
      }
      return r && r.documentElement && !r.getElementsByTagName("parsererror").length || x.error("Invalid XML: " + n), r
    },
    noop: function () {},
    globalEval: function (t) {
      t && x.trim(t) && (e.execScript || function (t) {
        e.eval.call(e, t)
      })(t)
    },
    camelCase: function (e) {
      return e.replace(D, "ms-").replace(L, H)
    },
    nodeName: function (e, t) {
      return e.nodeName && e.nodeName.toLowerCase() === t.toLowerCase()
    },
    each: function (e, t, n) {
      var r, i = 0,
        o = e.length,
        a = M(e);
      if (n) {
        if (a) {
          for (; o > i; i++)
            if (r = t.apply(e[i], n), r === !1) break
        } else
          for (i in e)
            if (r = t.apply(e[i], n), r === !1) break
      } else if (a) {
        for (; o > i; i++)
          if (r = t.call(e[i], i, e[i]), r === !1) break
      } else
        for (i in e)
          if (r = t.call(e[i], i, e[i]), r === !1) break;
      return e
    },
    trim: b && !b.call("? ") ? function (e) {
      return null == e ? "" : b.call(e)
    } : function (e) {
      return null == e ? "" : (e + "").replace(C, "")
    },
    makeArray: function (e, t) {
      var n = t || [];
      return null != e && (M(Object(e)) ? x.merge(n, "string" == typeof e ? [e] : e) : h.call(n, e)), n
    },
    inArray: function (e, t, n) {
      var r;
      if (t) {
        if (m) return m.call(t, e, n);
        for (r = t.length, n = n ? 0 > n ? Math.max(0, r + n) : n : 0; r > n; n++)
          if (n in t && t[n] === e) return n
      }
      return -1
    },
    merge: function (e, n) {
      var r = n.length,
        i = e.length,
        o = 0;
      if ("number" == typeof r)
        for (; r > o; o++) e[i++] = n[o];
      else
        while (n[o] !== t) e[i++] = n[o++];
      return e.length = i, e
    },
    grep: function (e, t, n) {
      var r, i = [],
        o = 0,
        a = e.length;
      for (n = !!n; a > o; o++) r = !!t(e[o], o), n !== r && i.push(e[o]);
      return i
    },
    map: function (e, t, n) {
      var r, i = 0,
        o = e.length,
        a = M(e),
        s = [];
      if (a)
        for (; o > i; i++) r = t(e[i], i, n), null != r && (s[s.length] = r);
      else
        for (i in e) r = t(e[i], i, n), null != r && (s[s.length] = r);
      return d.apply([], s)
    },
    guid: 1,
    proxy: function (e, n) {
      var r, i, o;
      return "string" == typeof n && (o = e[n], n = e, e = o), x.isFunction(e) ? (r = g.call(arguments, 2), i = function () {
        return e.apply(n || this, r.concat(g.call(arguments)))
      }, i.guid = e.guid = e.guid || x.guid++, i) : t
    },
    access: function (e, n, r, i, o, a, s) {
      var l = 0,
        u = e.length,
        c = null == r;
      if ("object" === x.type(r)) {
        o = !0;
        for (l in r) x.access(e, n, l, r[l], !0, a, s)
      } else if (i !== t && (o = !0, x.isFunction(i) || (s = !0), c && (s ? (n.call(e, i), n = null) : (c = n, n = function (e, t, n) {
          return c.call(x(e), n)
        })), n))
        for (; u > l; l++) n(e[l], r, s ? i : i.call(e[l], l, n(e[l], r)));
      return o ? e : c ? n.call(e) : u ? n(e[0], r) : a
    },
    now: function () {
      return (new Date).getTime()
    },
    swap: function (e, t, n, r) {
      var i, o, a = {};
      for (o in t) a[o] = e.style[o], e.style[o] = t[o];
      i = n.apply(e, r || []);
      for (o in t) e.style[o] = a[o];
      return i
    }
  }), x.ready.promise = function (t) {
    if (!n)
      if (n = x.Deferred(), "complete" === a.readyState) setTimeout(x.ready);
      else if (a.addEventListener) a.addEventListener("DOMContentLoaded", q, !1), e.addEventListener("load", q, !1);
    else {
      a.attachEvent("onreadystatechange", q), e.attachEvent("onload", q);
      var r = !1;
      try {
        r = null == e.frameElement && a.documentElement
      } catch (i) {}
      r && r.doScroll && function o() {
        if (!x.isReady) {
          try {
            r.doScroll("left")
          } catch (e) {
            return setTimeout(o, 50)
          }
          _(), x.ready()
        }
      }()
    }
    return n.promise(t)
  }, x.each("Boolean Number String Function Array Date RegExp Object Error".split(" "), function (e, t) {
    c["[object " + t + "]"] = t.toLowerCase()
  });

  function M(e) {
    var t = e.length,
      n = x.type(e);
    return x.isWindow(e) ? !1 : 1 === e.nodeType && t ? !0 : "array" === n || "function" !== n && (0 === t || "number" == typeof t && t > 0 && t - 1 in e)
  }
  r = x(a),
    function (e, t) {
      var n, r, i, o, a, s, l, u, c, p, f, d, h, g, m, y, v, b = "sizzle" + -new Date,
        w = e.document,
        T = 0,
        C = 0,
        N = st(),
        k = st(),
        E = st(),
        S = !1,
        A = function (e, t) {
          return e === t ? (S = !0, 0) : 0
        },
        j = typeof t,
        D = 1 << 31,
        L = {}.hasOwnProperty,
        H = [],
        q = H.pop,
        _ = H.push,
        M = H.push,
        O = H.slice,
        F = H.indexOf || function (e) {
          var t = 0,
            n = this.length;
          for (; n > t; t++)
            if (this[t] === e) return t;
          return -1
        },
        B = "checked|selected|async|autofocus|autoplay|controls|defer|disabled|hidden|ismap|loop|multiple|open|readonly|required|scoped",
        P = "[\\x20\\t\\r\\n\\f]",
        R = "(?:\\\\.|[\\w-]|[^\\x00-\\xa0])+",
        W = R.replace("w", "w#"),
        $ = "\\[" + P + "*(" + R + ")" + P + "*(?:([*^$|!~]?=)" + P + "*(?:(['\"])((?:\\\\.|[^\\\\])*?)\\3|(" + W + ")|)|)" + P + "*\\]",
        I = ":(" + R + ")(?:\\(((['\"])((?:\\\\.|[^\\\\])*?)\\3|((?:\\\\.|[^\\\\()[\\]]|" + $.replace(3, 8) + ")*)|.*)\\)|)",
        z = RegExp("^" + P + "+|((?:^|[^\\\\])(?:\\\\.)*)" + P + "+$", "g"),
        X = RegExp("^" + P + "*," + P + "*"),
        U = RegExp("^" + P + "*([>+~]|" + P + ")" + P + "*"),
        V = RegExp(P + "*[+~]"),
        Y = RegExp("=" + P + "*([^\\]'\"]*)" + P + "*\\]", "g"),
        J = RegExp(I),
        G = RegExp("^" + W + "$"),
        Q = {
          ID: RegExp("^#(" + R + ")"),
          CLASS: RegExp("^\\.(" + R + ")"),
          TAG: RegExp("^(" + R.replace("w", "w*") + ")"),
          ATTR: RegExp("^" + $),
          PSEUDO: RegExp("^" + I),
          CHILD: RegExp("^:(only|first|last|nth|nth-last)-(child|of-type)(?:\\(" + P + "*(even|odd|(([+-]|)(\\d*)n|)" + P + "*(?:([+-]|)" + P + "*(\\d+)|))" + P + "*\\)|)", "i"),
          bool: RegExp("^(?:" + B + ")$", "i"),
          needsContext: RegExp("^" + P + "*[>+~]|:(even|odd|eq|gt|lt|nth|first|last)(?:\\(" + P + "*((?:-\\d)?\\d*)" + P + "*\\)|)(?=[^-]|$)", "i")
        },
        K = /^[^{]+\{\s*\[native \w/,
        Z = /^(?:#([\w-]+)|(\w+)|\.([\w-]+))$/,
        et = /^(?:input|select|textarea|button)$/i,
        tt = /^h\d$/i,
        nt = /'|\\/g,
        rt = RegExp("\\\\([\\da-f]{1,6}" + P + "?|(" + P + ")|.)", "ig"),
        it = function (e, t, n) {
          var r = "0x" + t - 65536;
          return r !== r || n ? t : 0 > r ? String.fromCharCode(r + 65536) : String.fromCharCode(55296 | r >> 10, 56320 | 1023 & r)
        };
      try {
        M.apply(H = O.call(w.childNodes), w.childNodes), H[w.childNodes.length].nodeType
      } catch (ot) {
        M = {
          apply: H.length ? function (e, t) {
            _.apply(e, O.call(t))
          } : function (e, t) {
            var n = e.length,
              r = 0;
            while (e[n++] = t[r++]);
            e.length = n - 1
          }
        }
      }

      function at(e, t, n, i) {
        var o, a, s, l, u, c, d, m, y, x;
        if ((t ? t.ownerDocument || t : w) !== f && p(t), t = t || f, n = n || [], !e || "string" != typeof e) return n;
        if (1 !== (l = t.nodeType) && 9 !== l) return [];
        if (h && !i) {
          if (o = Z.exec(e))
            if (s = o[1]) {
              if (9 === l) {
                if (a = t.getElementById(s), !a || !a.parentNode) return n;
                if (a.id === s) return n.push(a), n
              } else if (t.ownerDocument && (a = t.ownerDocument.getElementById(s)) && v(t, a) && a.id === s) return n.push(a), n
            } else {
              if (o[2]) return M.apply(n, t.getElementsByTagName(e)), n;
              if ((s = o[3]) && r.getElementsByClassName && t.getElementsByClassName) return M.apply(n, t.getElementsByClassName(s)), n
            }
          if (r.qsa && (!g || !g.test(e))) {
            if (m = d = b, y = t, x = 9 === l && e, 1 === l && "object" !== t.nodeName.toLowerCase()) {
              c = mt(e), (d = t.getAttribute("id")) ? m = d.replace(nt, "\\$&") : t.setAttribute("id", m), m = "[id='" + m + "'] ", u = c.length;
              while (u--) c[u] = m + yt(c[u]);
              y = V.test(e) && t.parentNode || t, x = c.join(",")
            }
            if (x) try {
              return M.apply(n, y.querySelectorAll(x)), n
            } catch (T) {} finally {
              d || t.removeAttribute("id")
            }
          }
        }
        return kt(e.replace(z, "$1"), t, n, i)
      }

      function st() {
        var e = [];

        function t(n, r) {
          return e.push(n += " ") > o.cacheLength && delete t[e.shift()], t[n] = r
        }
        return t
      }

      function lt(e) {
        return e[b] = !0, e
      }

      function ut(e) {
        var t = f.createElement("div");
        try {
          return !!e(t)
        } catch (n) {
          return !1
        } finally {
          t.parentNode && t.parentNode.removeChild(t), t = null
        }
      }

      function ct(e, t) {
        var n = e.split("|"),
          r = e.length;
        while (r--) o.attrHandle[n[r]] = t
      }

      function pt(e, t) {
        var n = t && e,
          r = n && 1 === e.nodeType && 1 === t.nodeType && (~t.sourceIndex || D) - (~e.sourceIndex || D);
        if (r) return r;
        if (n)
          while (n = n.nextSibling)
            if (n === t) return -1;
        return e ? 1 : -1
      }

      function ft(e) {
        return function (t) {
          var n = t.nodeName.toLowerCase();
          return "input" === n && t.type === e
        }
      }

      function dt(e) {
        return function (t) {
          var n = t.nodeName.toLowerCase();
          return ("input" === n || "button" === n) && t.type === e
        }
      }

      function ht(e) {
        return lt(function (t) {
          return t = +t, lt(function (n, r) {
            var i, o = e([], n.length, t),
              a = o.length;
            while (a--) n[i = o[a]] && (n[i] = !(r[i] = n[i]))
          })
        })
      }
      s = at.isXML = function (e) {
        var t = e && (e.ownerDocument || e).documentElement;
        return t ? "HTML" !== t.nodeName : !1
      }, r = at.support = {}, p = at.setDocument = function (e) {
        var n = e ? e.ownerDocument || e : w,
          i = n.defaultView;
        return n !== f && 9 === n.nodeType && n.documentElement ? (f = n, d = n.documentElement, h = !s(n), i && i.attachEvent && i !== i.top && i.attachEvent("onbeforeunload", function () {
          p()
        }), r.attributes = ut(function (e) {
          return e.className = "i", !e.getAttribute("className")
        }), r.getElementsByTagName = ut(function (e) {
          return e.appendChild(n.createComment("")), !e.getElementsByTagName("*").length
        }), r.getElementsByClassName = ut(function (e) {
          return e.innerHTML = "<div class='a'></div><div class='a i'></div>", e.firstChild.className = "i", 2 === e.getElementsByClassName("i").length
        }), r.getById = ut(function (e) {
          return d.appendChild(e).id = b, !n.getElementsByName || !n.getElementsByName(b).length
        }), r.getById ? (o.find.ID = function (e, t) {
          if (typeof t.getElementById !== j && h) {
            var n = t.getElementById(e);
            return n && n.parentNode ? [n] : []
          }
        }, o.filter.ID = function (e) {
          var t = e.replace(rt, it);
          return function (e) {
            return e.getAttribute("id") === t
          }
        }) : (delete o.find.ID, o.filter.ID = function (e) {
          var t = e.replace(rt, it);
          return function (e) {
            var n = typeof e.getAttributeNode !== j && e.getAttributeNode("id");
            return n && n.value === t
          }
        }), o.find.TAG = r.getElementsByTagName ? function (e, n) {
          return typeof n.getElementsByTagName !== j ? n.getElementsByTagName(e) : t
        } : function (e, t) {
          var n, r = [],
            i = 0,
            o = t.getElementsByTagName(e);
          if ("*" === e) {
            while (n = o[i++]) 1 === n.nodeType && r.push(n);
            return r
          }
          return o
        }, o.find.CLASS = r.getElementsByClassName && function (e, n) {
          return typeof n.getElementsByClassName !== j && h ? n.getElementsByClassName(e) : t
        }, m = [], g = [], (r.qsa = K.test(n.querySelectorAll)) && (ut(function (e) {
          e.innerHTML = "<select><option selected=''></option></select>", e.querySelectorAll("[selected]").length || g.push("\\[" + P + "*(?:value|" + B + ")"), e.querySelectorAll(":checked").length || g.push(":checked")
        }), ut(function (e) {
          var t = n.createElement("input");
          t.setAttribute("type", "hidden"), e.appendChild(t).setAttribute("t", ""), e.querySelectorAll("[t^='']").length && g.push("[*^$]=" + P + "*(?:''|\"\")"), e.querySelectorAll(":enabled").length || g.push(":enabled", ":disabled"), e.querySelectorAll("*,:x"), g.push(",.*:")
        })), (r.matchesSelector = K.test(y = d.webkitMatchesSelector || d.mozMatchesSelector || d.oMatchesSelector || d.msMatchesSelector)) && ut(function (e) {
          r.disconnectedMatch = y.call(e, "div"), y.call(e, "[s!='']:x"), m.push("!=", I)
        }), g = g.length && RegExp(g.join("|")), m = m.length && RegExp(m.join("|")), v = K.test(d.contains) || d.compareDocumentPosition ? function (e, t) {
          var n = 9 === e.nodeType ? e.documentElement : e,
            r = t && t.parentNode;
          return e === r || !(!r || 1 !== r.nodeType || !(n.contains ? n.contains(r) : e.compareDocumentPosition && 16 & e.compareDocumentPosition(r)))
        } : function (e, t) {
          if (t)
            while (t = t.parentNode)
              if (t === e) return !0;
          return !1
        }, A = d.compareDocumentPosition ? function (e, t) {
          if (e === t) return S = !0, 0;
          var i = t.compareDocumentPosition && e.compareDocumentPosition && e.compareDocumentPosition(t);
          return i ? 1 & i || !r.sortDetached && t.compareDocumentPosition(e) === i ? e === n || v(w, e) ? -1 : t === n || v(w, t) ? 1 : c ? F.call(c, e) - F.call(c, t) : 0 : 4 & i ? -1 : 1 : e.compareDocumentPosition ? -1 : 1
        } : function (e, t) {
          var r, i = 0,
            o = e.parentNode,
            a = t.parentNode,
            s = [e],
            l = [t];
          if (e === t) return S = !0, 0;
          if (!o || !a) return e === n ? -1 : t === n ? 1 : o ? -1 : a ? 1 : c ? F.call(c, e) - F.call(c, t) : 0;
          if (o === a) return pt(e, t);
          r = e;
          while (r = r.parentNode) s.unshift(r);
          r = t;
          while (r = r.parentNode) l.unshift(r);
          while (s[i] === l[i]) i++;
          return i ? pt(s[i], l[i]) : s[i] === w ? -1 : l[i] === w ? 1 : 0
        }, n) : f
      }, at.matches = function (e, t) {
        return at(e, null, null, t)
      }, at.matchesSelector = function (e, t) {
        if ((e.ownerDocument || e) !== f && p(e), t = t.replace(Y, "='$1']"), !(!r.matchesSelector || !h || m && m.test(t) || g && g.test(t))) try {
          var n = y.call(e, t);
          if (n || r.disconnectedMatch || e.document && 11 !== e.document.nodeType) return n
        } catch (i) {}
        return at(t, f, null, [e]).length > 0
      }, at.contains = function (e, t) {
        return (e.ownerDocument || e) !== f && p(e), v(e, t)
      }, at.attr = function (e, n) {
        (e.ownerDocument || e) !== f && p(e);
        var i = o.attrHandle[n.toLowerCase()],
          a = i && L.call(o.attrHandle, n.toLowerCase()) ? i(e, n, !h) : t;
        return a === t ? r.attributes || !h ? e.getAttribute(n) : (a = e.getAttributeNode(n)) && a.specified ? a.value : null : a
      }, at.error = function (e) {
        throw Error("Syntax error, unrecognized expression: " + e)
      }, at.uniqueSort = function (e) {
        var t, n = [],
          i = 0,
          o = 0;
        if (S = !r.detectDuplicates, c = !r.sortStable && e.slice(0), e.sort(A), S) {
          while (t = e[o++]) t === e[o] && (i = n.push(o));
          while (i--) e.splice(n[i], 1)
        }
        return e
      }, a = at.getText = function (e) {
        var t, n = "",
          r = 0,
          i = e.nodeType;
        if (i) {
          if (1 === i || 9 === i || 11 === i) {
            if ("string" == typeof e.textContent) return e.textContent;
            for (e = e.firstChild; e; e = e.nextSibling) n += a(e)
          } else if (3 === i || 4 === i) return e.nodeValue
        } else
          for (; t = e[r]; r++) n += a(t);
        return n
      }, o = at.selectors = {
        cacheLength: 50,
        createPseudo: lt,
        match: Q,
        attrHandle: {},
        find: {},
        relative: {
          ">": {
            dir: "parentNode",
            first: !0
          },
          " ": {
            dir: "parentNode"
          },
          "+": {
            dir: "previousSibling",
            first: !0
          },
          "~": {
            dir: "previousSibling"
          }
        },
        preFilter: {
          ATTR: function (e) {
            return e[1] = e[1].replace(rt, it), e[3] = (e[4] || e[5] || "").replace(rt, it), "~=" === e[2] && (e[3] = " " + e[3] + " "), e.slice(0, 4)
          },
          CHILD: function (e) {
            return e[1] = e[1].toLowerCase(), "nth" === e[1].slice(0, 3) ? (e[3] || at.error(e[0]), e[4] = +(e[4] ? e[5] + (e[6] || 1) : 2 * ("even" === e[3] || "odd" === e[3])), e[5] = +(e[7] + e[8] || "odd" === e[3])) : e[3] && at.error(e[0]), e
          },
          PSEUDO: function (e) {
            var n, r = !e[5] && e[2];
            return Q.CHILD.test(e[0]) ? null : (e[3] && e[4] !== t ? e[2] = e[4] : r && J.test(r) && (n = mt(r, !0)) && (n = r.indexOf(")", r.length - n) - r.length) && (e[0] = e[0].slice(0, n), e[2] = r.slice(0, n)), e.slice(0, 3))
          }
        },
        filter: {
          TAG: function (e) {
            var t = e.replace(rt, it).toLowerCase();
            return "*" === e ? function () {
              return !0
            } : function (e) {
              return e.nodeName && e.nodeName.toLowerCase() === t
            }
          },
          CLASS: function (e) {
            var t = N[e + " "];
            return t || (t = RegExp("(^|" + P + ")" + e + "(" + P + "|$)")) && N(e, function (e) {
              return t.test("string" == typeof e.className && e.className || typeof e.getAttribute !== j && e.getAttribute("class") || "")
            })
          },
          ATTR: function (e, t, n) {
            return function (r) {
              var i = at.attr(r, e);
              return null == i ? "!=" === t : t ? (i += "", "=" === t ? i === n : "!=" === t ? i !== n : "^=" === t ? n && 0 === i.indexOf(n) : "*=" === t ? n && i.indexOf(n) > -1 : "$=" === t ? n && i.slice(-n.length) === n : "~=" === t ? (" " + i + " ").indexOf(n) > -1 : "|=" === t ? i === n || i.slice(0, n.length + 1) === n + "-" : !1) : !0
            }
          },
          CHILD: function (e, t, n, r, i) {
            var o = "nth" !== e.slice(0, 3),
              a = "last" !== e.slice(-4),
              s = "of-type" === t;
            return 1 === r && 0 === i ? function (e) {
              return !!e.parentNode
            } : function (t, n, l) {
              var u, c, p, f, d, h, g = o !== a ? "nextSibling" : "previousSibling",
                m = t.parentNode,
                y = s && t.nodeName.toLowerCase(),
                v = !l && !s;
              if (m) {
                if (o) {
                  while (g) {
                    p = t;
                    while (p = p[g])
                      if (s ? p.nodeName.toLowerCase() === y : 1 === p.nodeType) return !1;
                    h = g = "only" === e && !h && "nextSibling"
                  }
                  return !0
                }
                if (h = [a ? m.firstChild : m.lastChild], a && v) {
                  c = m[b] || (m[b] = {}), u = c[e] || [], d = u[0] === T && u[1], f = u[0] === T && u[2], p = d && m.childNodes[d];
                  while (p = ++d && p && p[g] || (f = d = 0) || h.pop())
                    if (1 === p.nodeType && ++f && p === t) {
                      c[e] = [T, d, f];
                      break
                    }
                } else if (v && (u = (t[b] || (t[b] = {}))[e]) && u[0] === T) f = u[1];
                else
                  while (p = ++d && p && p[g] || (f = d = 0) || h.pop())
                    if ((s ? p.nodeName.toLowerCase() === y : 1 === p.nodeType) && ++f && (v && ((p[b] || (p[b] = {}))[e] = [T, f]), p === t)) break;
                return f -= i, f === r || 0 === f % r && f / r >= 0
              }
            }
          },
          PSEUDO: function (e, t) {
            var n, r = o.pseudos[e] || o.setFilters[e.toLowerCase()] || at.error("unsupported pseudo: " + e);
            return r[b] ? r(t) : r.length > 1 ? (n = [e, e, "", t], o.setFilters.hasOwnProperty(e.toLowerCase()) ? lt(function (e, n) {
              var i, o = r(e, t),
                a = o.length;
              while (a--) i = F.call(e, o[a]), e[i] = !(n[i] = o[a])
            }) : function (e) {
              return r(e, 0, n)
            }) : r
          }
        },
        pseudos: {
          not: lt(function (e) {
            var t = [],
              n = [],
              r = l(e.replace(z, "$1"));
            return r[b] ? lt(function (e, t, n, i) {
              var o, a = r(e, null, i, []),
                s = e.length;
              while (s--)(o = a[s]) && (e[s] = !(t[s] = o))
            }) : function (e, i, o) {
              return t[0] = e, r(t, null, o, n), !n.pop()
            }
          }),
          has: lt(function (e) {
            return function (t) {
              return at(e, t).length > 0
            }
          }),
          contains: lt(function (e) {
            return function (t) {
              return (t.textContent || t.innerText || a(t)).indexOf(e) > -1
            }
          }),
          lang: lt(function (e) {
            return G.test(e || "") || at.error("unsupported lang: " + e), e = e.replace(rt, it).toLowerCase(),
              function (t) {
                var n;
                do
                  if (n = h ? t.lang : t.getAttribute("xml:lang") || t.getAttribute("lang")) return n = n.toLowerCase(), n === e || 0 === n.indexOf(e + "-"); while ((t = t.parentNode) && 1 === t.nodeType);
                return !1
              }
          }),
          target: function (t) {
            var n = e.location && e.location.hash;
            return n && n.slice(1) === t.id
          },
          root: function (e) {
            return e === d
          },
          focus: function (e) {
            return e === f.activeElement && (!f.hasFocus || f.hasFocus()) && !!(e.type || e.href || ~e.tabIndex)
          },
          enabled: function (e) {
            return e.disabled === !1
          },
          disabled: function (e) {
            return e.disabled === !0
          },
          checked: function (e) {
            var t = e.nodeName.toLowerCase();
            return "input" === t && !!e.checked || "option" === t && !!e.selected
          },
          selected: function (e) {
            return e.parentNode && e.parentNode.selectedIndex, e.selected === !0
          },
          empty: function (e) {
            for (e = e.firstChild; e; e = e.nextSibling)
              if (e.nodeName > "@" || 3 === e.nodeType || 4 === e.nodeType) return !1;
            return !0
          },
          parent: function (e) {
            return !o.pseudos.empty(e)
          },
          header: function (e) {
            return tt.test(e.nodeName)
          },
          input: function (e) {
            return et.test(e.nodeName)
          },
          button: function (e) {
            var t = e.nodeName.toLowerCase();
            return "input" === t && "button" === e.type || "button" === t
          },
          text: function (e) {
            var t;
            return "input" === e.nodeName.toLowerCase() && "text" === e.type && (null == (t = e.getAttribute("type")) || t.toLowerCase() === e.type)
          },
          first: ht(function () {
            return [0]
          }),
          last: ht(function (e, t) {
            return [t - 1]
          }),
          eq: ht(function (e, t, n) {
            return [0 > n ? n + t : n]
          }),
          even: ht(function (e, t) {
            var n = 0;
            for (; t > n; n += 2) e.push(n);
            return e
          }),
          odd: ht(function (e, t) {
            var n = 1;
            for (; t > n; n += 2) e.push(n);
            return e
          }),
          lt: ht(function (e, t, n) {
            var r = 0 > n ? n + t : n;
            for (; --r >= 0;) e.push(r);
            return e
          }),
          gt: ht(function (e, t, n) {
            var r = 0 > n ? n + t : n;
            for (; t > ++r;) e.push(r);
            return e
          })
        }
      }, o.pseudos.nth = o.pseudos.eq;
      for (n in {
          radio: !0,
          checkbox: !0,
          file: !0,
          password: !0,
          image: !0
        }) o.pseudos[n] = ft(n);
      for (n in {
          submit: !0,
          reset: !0
        }) o.pseudos[n] = dt(n);

      function gt() {}
      gt.prototype = o.filters = o.pseudos, o.setFilters = new gt;

      function mt(e, t) {
        var n, r, i, a, s, l, u, c = k[e + " "];
        if (c) return t ? 0 : c.slice(0);
        s = e, l = [], u = o.preFilter;
        while (s) {
          (!n || (r = X.exec(s))) && (r && (s = s.slice(r[0].length) || s), l.push(i = [])), n = !1, (r = U.exec(s)) && (n = r.shift(), i.push({
            value: n,
            type: r[0].replace(z, " ")
          }), s = s.slice(n.length));
          for (a in o.filter) !(r = Q[a].exec(s)) || u[a] && !(r = u[a](r)) || (n = r.shift(), i.push({
            value: n,
            type: a,
            matches: r
          }), s = s.slice(n.length));
          if (!n) break
        }
        return t ? s.length : s ? at.error(e) : k(e, l).slice(0)
      }

      function yt(e) {
        var t = 0,
          n = e.length,
          r = "";
        for (; n > t; t++) r += e[t].value;
        return r
      }

      function vt(e, t, n) {
        var r = t.dir,
          o = n && "parentNode" === r,
          a = C++;
        return t.first ? function (t, n, i) {
          while (t = t[r])
            if (1 === t.nodeType || o) return e(t, n, i)
        } : function (t, n, s) {
          var l, u, c, p = T + " " + a;
          if (s) {
            while (t = t[r])
              if ((1 === t.nodeType || o) && e(t, n, s)) return !0
          } else
            while (t = t[r])
              if (1 === t.nodeType || o)
                if (c = t[b] || (t[b] = {}), (u = c[r]) && u[0] === p) {
                  if ((l = u[1]) === !0 || l === i) return l === !0
                } else if (u = c[r] = [p], u[1] = e(t, n, s) || i, u[1] === !0) return !0
        }
      }

      function bt(e) {
        return e.length > 1 ? function (t, n, r) {
          var i = e.length;
          while (i--)
            if (!e[i](t, n, r)) return !1;
          return !0
        } : e[0]
      }

      function xt(e, t, n, r, i) {
        var o, a = [],
          s = 0,
          l = e.length,
          u = null != t;
        for (; l > s; s++)(o = e[s]) && (!n || n(o, r, i)) && (a.push(o), u && t.push(s));
        return a
      }

      function wt(e, t, n, r, i, o) {
        return r && !r[b] && (r = wt(r)), i && !i[b] && (i = wt(i, o)), lt(function (o, a, s, l) {
          var u, c, p, f = [],
            d = [],
            h = a.length,
            g = o || Nt(t || "*", s.nodeType ? [s] : s, []),
            m = !e || !o && t ? g : xt(g, f, e, s, l),
            y = n ? i || (o ? e : h || r) ? [] : a : m;
          if (n && n(m, y, s, l), r) {
            u = xt(y, d), r(u, [], s, l), c = u.length;
            while (c--)(p = u[c]) && (y[d[c]] = !(m[d[c]] = p))
          }
          if (o) {
            if (i || e) {
              if (i) {
                u = [], c = y.length;
                while (c--)(p = y[c]) && u.push(m[c] = p);
                i(null, y = [], u, l)
              }
              c = y.length;
              while (c--)(p = y[c]) && (u = i ? F.call(o, p) : f[c]) > -1 && (o[u] = !(a[u] = p))
            }
          } else y = xt(y === a ? y.splice(h, y.length) : y), i ? i(null, a, y, l) : M.apply(a, y)
        })
      }

      function Tt(e) {
        var t, n, r, i = e.length,
          a = o.relative[e[0].type],
          s = a || o.relative[" "],
          l = a ? 1 : 0,
          c = vt(function (e) {
            return e === t
          }, s, !0),
          p = vt(function (e) {
            return F.call(t, e) > -1
          }, s, !0),
          f = [function (e, n, r) {
            return !a && (r || n !== u) || ((t = n).nodeType ? c(e, n, r) : p(e, n, r))
          }];
        for (; i > l; l++)
          if (n = o.relative[e[l].type]) f = [vt(bt(f), n)];
          else {
            if (n = o.filter[e[l].type].apply(null, e[l].matches), n[b]) {
              for (r = ++l; i > r; r++)
                if (o.relative[e[r].type]) break;
              return wt(l > 1 && bt(f), l > 1 && yt(e.slice(0, l - 1).concat({
                value: " " === e[l - 2].type ? "*" : ""
              })).replace(z, "$1"), n, r > l && Tt(e.slice(l, r)), i > r && Tt(e = e.slice(r)), i > r && yt(e))
            }
            f.push(n)
          }
        return bt(f)
      }

      function Ct(e, t) {
        var n = 0,
          r = t.length > 0,
          a = e.length > 0,
          s = function (s, l, c, p, d) {
            var h, g, m, y = [],
              v = 0,
              b = "0",
              x = s && [],
              w = null != d,
              C = u,
              N = s || a && o.find.TAG("*", d && l.parentNode || l),
              k = T += null == C ? 1 : Math.random() || .1;
            for (w && (u = l !== f && l, i = n); null != (h = N[b]); b++) {
              if (a && h) {
                g = 0;
                while (m = e[g++])
                  if (m(h, l, c)) {
                    p.push(h);
                    break
                  }
                w && (T = k, i = ++n)
              }
              r && ((h = !m && h) && v--, s && x.push(h))
            }
            if (v += b, r && b !== v) {
              g = 0;
              while (m = t[g++]) m(x, y, l, c);
              if (s) {
                if (v > 0)
                  while (b--) x[b] || y[b] || (y[b] = q.call(p));
                y = xt(y)
              }
              M.apply(p, y), w && !s && y.length > 0 && v + t.length > 1 && at.uniqueSort(p)
            }
            return w && (T = k, u = C), x
          };
        return r ? lt(s) : s
      }
      l = at.compile = function (e, t) {
        var n, r = [],
          i = [],
          o = E[e + " "];
        if (!o) {
          t || (t = mt(e)), n = t.length;
          while (n--) o = Tt(t[n]), o[b] ? r.push(o) : i.push(o);
          o = E(e, Ct(i, r))
        }
        return o
      };

      function Nt(e, t, n) {
        var r = 0,
          i = t.length;
        for (; i > r; r++) at(e, t[r], n);
        return n
      }

      function kt(e, t, n, i) {
        var a, s, u, c, p, f = mt(e);
        if (!i && 1 === f.length) {
          if (s = f[0] = f[0].slice(0), s.length > 2 && "ID" === (u = s[0]).type && r.getById && 9 === t.nodeType && h && o.relative[s[1].type]) {
            if (t = (o.find.ID(u.matches[0].replace(rt, it), t) || [])[0], !t) return n;
            e = e.slice(s.shift().value.length)
          }
          a = Q.needsContext.test(e) ? 0 : s.length;
          while (a--) {
            if (u = s[a], o.relative[c = u.type]) break;
            if ((p = o.find[c]) && (i = p(u.matches[0].replace(rt, it), V.test(s[0].type) && t.parentNode || t))) {
              if (s.splice(a, 1), e = i.length && yt(s), !e) return M.apply(n, i), n;
              break
            }
          }
        }
        return l(e, f)(i, t, !h, n, V.test(e)), n
      }
      r.sortStable = b.split("").sort(A).join("") === b, r.detectDuplicates = S, p(), r.sortDetached = ut(function (e) {
        return 1 & e.compareDocumentPosition(f.createElement("div"))
      }), ut(function (e) {
        return e.innerHTML = "<a href='#'></a>", "#" === e.firstChild.getAttribute("href")
      }) || ct("type|href|height|width", function (e, n, r) {
        return r ? t : e.getAttribute(n, "type" === n.toLowerCase() ? 1 : 2)
      }), r.attributes && ut(function (e) {
        return e.innerHTML = "<input/>", e.firstChild.setAttribute("value", ""), "" === e.firstChild.getAttribute("value")
      }) || ct("value", function (e, n, r) {
        return r || "input" !== e.nodeName.toLowerCase() ? t : e.defaultValue
      }), ut(function (e) {
        return null == e.getAttribute("disabled")
      }) || ct(B, function (e, n, r) {
        var i;
        return r ? t : (i = e.getAttributeNode(n)) && i.specified ? i.value : e[n] === !0 ? n.toLowerCase() : null
      }), x.find = at, x.expr = at.selectors, x.expr[":"] = x.expr.pseudos, x.unique = at.uniqueSort, x.text = at.getText, x.isXMLDoc = at.isXML, x.contains = at.contains
    }(e);
  var O = {};

  function F(e) {
    var t = O[e] = {};
    return x.each(e.match(T) || [], function (e, n) {
      t[n] = !0
    }), t
  }
  x.Callbacks = function (e) {
    e = "string" == typeof e ? O[e] || F(e) : x.extend({}, e);
    var n, r, i, o, a, s, l = [],
      u = !e.once && [],
      c = function (t) {
        for (r = e.memory && t, i = !0, a = s || 0, s = 0, o = l.length, n = !0; l && o > a; a++)
          if (l[a].apply(t[0], t[1]) === !1 && e.stopOnFalse) {
            r = !1;
            break
          }
        n = !1, l && (u ? u.length && c(u.shift()) : r ? l = [] : p.disable())
      },
      p = {
        add: function () {
          if (l) {
            var t = l.length;
            (function i(t) {
              x.each(t, function (t, n) {
                var r = x.type(n);
                "function" === r ? e.unique && p.has(n) || l.push(n) : n && n.length && "string" !== r && i(n)
              })
            })(arguments), n ? o = l.length : r && (s = t, c(r))
          }
          return this
        },
        remove: function () {
          return l && x.each(arguments, function (e, t) {
            var r;
            while ((r = x.inArray(t, l, r)) > -1) l.splice(r, 1), n && (o >= r && o--, a >= r && a--)
          }), this
        },
        has: function (e) {
          return e ? x.inArray(e, l) > -1 : !(!l || !l.length)
        },
        empty: function () {
          return l = [], o = 0, this
        },
        disable: function () {
          return l = u = r = t, this
        },
        disabled: function () {
          return !l
        },
        lock: function () {
          return u = t, r || p.disable(), this
        },
        locked: function () {
          return !u
        },
        fireWith: function (e, t) {
          return !l || i && !u || (t = t || [], t = [e, t.slice ? t.slice() : t], n ? u.push(t) : c(t)), this
        },
        fire: function () {
          return p.fireWith(this, arguments), this
        },
        fired: function () {
          return !!i
        }
      };
    return p
  }, x.extend({
    Deferred: function (e) {
      var t = [
          ["resolve", "done", x.Callbacks("once memory"), "resolved"],
          ["reject", "fail", x.Callbacks("once memory"), "rejected"],
          ["notify", "progress", x.Callbacks("memory")]
        ],
        n = "pending",
        r = {
          state: function () {
            return n
          },
          always: function () {
            return i.done(arguments).fail(arguments), this
          },
          then: function () {
            var e = arguments;
            return x.Deferred(function (n) {
              x.each(t, function (t, o) {
                var a = o[0],
                  s = x.isFunction(e[t]) && e[t];
                i[o[1]](function () {
                  var e = s && s.apply(this, arguments);
                  e && x.isFunction(e.promise) ? e.promise().done(n.resolve).fail(n.reject).progress(n.notify) : n[a + "With"](this === r ? n.promise() : this, s ? [e] : arguments)
                })
              }), e = null
            }).promise()
          },
          promise: function (e) {
            return null != e ? x.extend(e, r) : r
          }
        },
        i = {};
      return r.pipe = r.then, x.each(t, function (e, o) {
        var a = o[2],
          s = o[3];
        r[o[1]] = a.add, s && a.add(function () {
          n = s
        }, t[1 ^ e][2].disable, t[2][2].lock), i[o[0]] = function () {
          return i[o[0] + "With"](this === i ? r : this, arguments), this
        }, i[o[0] + "With"] = a.fireWith
      }), r.promise(i), e && e.call(i, i), i
    },
    when: function (e) {
      var t = 0,
        n = g.call(arguments),
        r = n.length,
        i = 1 !== r || e && x.isFunction(e.promise) ? r : 0,
        o = 1 === i ? e : x.Deferred(),
        a = function (e, t, n) {
          return function (r) {
            t[e] = this, n[e] = arguments.length > 1 ? g.call(arguments) : r, n === s ? o.notifyWith(t, n) : --i || o.resolveWith(t, n)
          }
        },
        s, l, u;
      if (r > 1)
        for (s = Array(r), l = Array(r), u = Array(r); r > t; t++) n[t] && x.isFunction(n[t].promise) ? n[t].promise().done(a(t, u, n)).fail(o.reject).progress(a(t, l, s)) : --i;
      return i || o.resolveWith(u, n), o.promise()
    }
  }), x.support = function (t) {
    var n, r, o, s, l, u, c, p, f, d = a.createElement("div");
    if (d.setAttribute("className", "t"), d.innerHTML = "  <link/><table></table><a href='/a'>a</a><input type='checkbox'/>", n = d.getElementsByTagName("*") || [], r = d.getElementsByTagName("a")[0], !r || !r.style || !n.length) return t;
    s = a.createElement("select"), u = s.appendChild(a.createElement("option")), o = d.getElementsByTagName("input")[0], r.style.cssText = "top:1px;float:left;opacity:.5", t.getSetAttribute = "t" !== d.className, t.leadingWhitespace = 3 === d.firstChild.nodeType, t.tbody = !d.getElementsByTagName("tbody").length, t.htmlSerialize = !!d.getElementsByTagName("link").length, t.style = /top/.test(r.getAttribute("style")), t.hrefNormalized = "/a" === r.getAttribute("href"), t.opacity = /^0.5/.test(r.style.opacity), t.cssFloat = !!r.style.cssFloat, t.checkOn = !!o.value, t.optSelected = u.selected, t.enctype = !!a.createElement("form").enctype, t.html5Clone = "<:nav></:nav>" !== a.createElement("nav").cloneNode(!0).outerHTML, t.inlineBlockNeedsLayout = !1, t.shrinkWrapBlocks = !1, t.pixelPosition = !1, t.deleteExpando = !0, t.noCloneEvent = !0, t.reliableMarginRight = !0, t.boxSizingReliable = !0, o.checked = !0, t.noCloneChecked = o.cloneNode(!0).checked, s.disabled = !0, t.optDisabled = !u.disabled;
    try {
      delete d.test
    } catch (h) {
      t.deleteExpando = !1
    }
    o = a.createElement("input"), o.setAttribute("value", ""), t.input = "" === o.getAttribute("value"), o.value = "t", o.setAttribute("type", "radio"), t.radioValue = "t" === o.value, o.setAttribute("checked", "t"), o.setAttribute("name", "t"), l = a.createDocumentFragment(), l.appendChild(o), t.appendChecked = o.checked, t.checkClone = l.cloneNode(!0).cloneNode(!0).lastChild.checked, d.attachEvent && (d.attachEvent("onclick", function () {
      t.noCloneEvent = !1
    }), d.cloneNode(!0).click());
    for (f in {
        submit: !0,
        change: !0,
        focusin: !0
      }) d.setAttribute(c = "on" + f, "t"), t[f + "Bubbles"] = c in e || d.attributes[c].expando === !1;
    d.style.backgroundClip = "content-box", d.cloneNode(!0).style.backgroundClip = "", t.clearCloneStyle = "content-box" === d.style.backgroundClip;
    for (f in x(t)) break;
    return t.ownLast = "0" !== f, x(function () {
      var n, r, o, s = "padding:0;margin:0;border:0;display:block;box-sizing:content-box;-moz-box-sizing:content-box;-webkit-box-sizing:content-box;",
        l = a.getElementsByTagName("body")[0];
      l && (n = a.createElement("div"), n.style.cssText = "border:0;width:0;height:0;position:absolute;top:0;left:-9999px;margin-top:1px", l.appendChild(n).appendChild(d), d.innerHTML = "<table><tr><td></td><td>t</td></tr></table>", o = d.getElementsByTagName("td"), o[0].style.cssText = "padding:0;margin:0;border:0;display:none", p = 0 === o[0].offsetHeight, o[0].style.display = "", o[1].style.display = "none", t.reliableHiddenOffsets = p && 0 === o[0].offsetHeight, d.innerHTML = "", d.style.cssText = "box-sizing:border-box;-moz-box-sizing:border-box;-webkit-box-sizing:border-box;padding:1px;border:1px;display:block;width:4px;margin-top:1%;position:absolute;top:1%;", x.swap(l, null != l.style.zoom ? {
        zoom: 1
      } : {}, function () {
        t.boxSizing = 4 === d.offsetWidth
      }), e.getComputedStyle && (t.pixelPosition = "1%" !== (e.getComputedStyle(d, null) || {}).top, t.boxSizingReliable = "4px" === (e.getComputedStyle(d, null) || {
        width: "4px"
      }).width, r = d.appendChild(a.createElement("div")), r.style.cssText = d.style.cssText = s, r.style.marginRight = r.style.width = "0", d.style.width = "1px", t.reliableMarginRight = !parseFloat((e.getComputedStyle(r, null) || {}).marginRight)), typeof d.style.zoom !== i && (d.innerHTML = "", d.style.cssText = s + "width:1px;padding:1px;display:inline;zoom:1", t.inlineBlockNeedsLayout = 3 === d.offsetWidth, d.style.display = "block", d.innerHTML = "<div></div>", d.firstChild.style.width = "5px", t.shrinkWrapBlocks = 3 !== d.offsetWidth, t.inlineBlockNeedsLayout && (l.style.zoom = 1)), l.removeChild(n), n = d = o = r = null)
    }), n = s = l = u = r = o = null, t
  }({});
  var B = /(?:\{[\s\S]*\}|\[[\s\S]*\])$/,
    P = /([A-Z])/g;

  function R(e, n, r, i) {
    if (x.acceptData(e)) {
      var o, a, s = x.expando,
        l = e.nodeType,
        u = l ? x.cache : e,
        c = l ? e[s] : e[s] && s;
      if (c && u[c] && (i || u[c].data) || r !== t || "string" != typeof n) return c || (c = l ? e[s] = p.pop() || x.guid++ : s), u[c] || (u[c] = l ? {} : {
        toJSON: x.noop
      }), ("object" == typeof n || "function" == typeof n) && (i ? u[c] = x.extend(u[c], n) : u[c].data = x.extend(u[c].data, n)), a = u[c], i || (a.data || (a.data = {}), a = a.data), r !== t && (a[x.camelCase(n)] = r), "string" == typeof n ? (o = a[n], null == o && (o = a[x.camelCase(n)])) : o = a, o
    }
  }

  function W(e, t, n) {
    if (x.acceptData(e)) {
      var r, i, o = e.nodeType,
        a = o ? x.cache : e,
        s = o ? e[x.expando] : x.expando;
      if (a[s]) {
        if (t && (r = n ? a[s] : a[s].data)) {
          x.isArray(t) ? t = t.concat(x.map(t, x.camelCase)) : t in r ? t = [t] : (t = x.camelCase(t), t = t in r ? [t] : t.split(" ")), i = t.length;
          while (i--) delete r[t[i]];
          if (n ? !I(r) : !x.isEmptyObject(r)) return
        }(n || (delete a[s].data, I(a[s]))) && (o ? x.cleanData([e], !0) : x.support.deleteExpando || a != a.window ? delete a[s] : a[s] = null)
      }
    }
  }
  x.extend({
    cache: {},
    noData: {
      applet: !0,
      embed: !0,
      object: "clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"
    },
    hasData: function (e) {
      return e = e.nodeType ? x.cache[e[x.expando]] : e[x.expando], !!e && !I(e)
    },
    data: function (e, t, n) {
      return R(e, t, n)
    },
    removeData: function (e, t) {
      return W(e, t)
    },
    _data: function (e, t, n) {
      return R(e, t, n, !0)
    },
    _removeData: function (e, t) {
      return W(e, t, !0)
    },
    acceptData: function (e) {
      if (e.nodeType && 1 !== e.nodeType && 9 !== e.nodeType) return !1;
      var t = e.nodeName && x.noData[e.nodeName.toLowerCase()];
      return !t || t !== !0 && e.getAttribute("classid") === t
    }
  }), x.fn.extend({
    data: function (e, n) {
      var r, i, o = null,
        a = 0,
        s = this[0];
      if (e === t) {
        if (this.length && (o = x.data(s), 1 === s.nodeType && !x._data(s, "parsedAttrs"))) {
          for (r = s.attributes; r.length > a; a++) i = r[a].name, 0 === i.indexOf("data-") && (i = x.camelCase(i.slice(5)), $(s, i, o[i]));
          x._data(s, "parsedAttrs", !0)
        }
        return o
      }
      return "object" == typeof e ? this.each(function () {
        x.data(this, e)
      }) : arguments.length > 1 ? this.each(function () {
        x.data(this, e, n)
      }) : s ? $(s, e, x.data(s, e)) : null
    },
    removeData: function (e) {
      return this.each(function () {
        x.removeData(this, e)
      })
    }
  });

  function $(e, n, r) {
    if (r === t && 1 === e.nodeType) {
      var i = "data-" + n.replace(P, "-$1").toLowerCase();
      if (r = e.getAttribute(i), "string" == typeof r) {
        try {
          r = "true" === r ? !0 : "false" === r ? !1 : "null" === r ? null : +r + "" === r ? +r : B.test(r) ? x.parseJSON(r) : r
        } catch (o) {}
        x.data(e, n, r)
      } else r = t
    }
    return r
  }

  function I(e) {
    var t;
    for (t in e)
      if (("data" !== t || !x.isEmptyObject(e[t])) && "toJSON" !== t) return !1;
    return !0
  }
  x.extend({
    queue: function (e, n, r) {
      var i;
      return e ? (n = (n || "fx") + "queue", i = x._data(e, n), r && (!i || x.isArray(r) ? i = x._data(e, n, x.makeArray(r)) : i.push(r)), i || []) : t
    },
    dequeue: function (e, t) {
      t = t || "fx";
      var n = x.queue(e, t),
        r = n.length,
        i = n.shift(),
        o = x._queueHooks(e, t),
        a = function () {
          x.dequeue(e, t)
        };
      "inprogress" === i && (i = n.shift(), r--), i && ("fx" === t && n.unshift("inprogress"), delete o.stop, i.call(e, a, o)), !r && o && o.empty.fire()
    },
    _queueHooks: function (e, t) {
      var n = t + "queueHooks";
      return x._data(e, n) || x._data(e, n, {
        empty: x.Callbacks("once memory").add(function () {
          x._removeData(e, t + "queue"), x._removeData(e, n)
        })
      })
    }
  }), x.fn.extend({
    queue: function (e, n) {
      var r = 2;
      return "string" != typeof e && (n = e, e = "fx", r--), r > arguments.length ? x.queue(this[0], e) : n === t ? this : this.each(function () {
        var t = x.queue(this, e, n);
        x._queueHooks(this, e), "fx" === e && "inprogress" !== t[0] && x.dequeue(this, e)
      })
    },
    dequeue: function (e) {
      return this.each(function () {
        x.dequeue(this, e)
      })
    },
    delay: function (e, t) {
      return e = x.fx ? x.fx.speeds[e] || e : e, t = t || "fx", this.queue(t, function (t, n) {
        var r = setTimeout(t, e);
        n.stop = function () {
          clearTimeout(r)
        }
      })
    },
    clearQueue: function (e) {
      return this.queue(e || "fx", [])
    },
    promise: function (e, n) {
      var r, i = 1,
        o = x.Deferred(),
        a = this,
        s = this.length,
        l = function () {
          --i || o.resolveWith(a, [a])
        };
      "string" != typeof e && (n = e, e = t), e = e || "fx";
      while (s--) r = x._data(a[s], e + "queueHooks"), r && r.empty && (i++, r.empty.add(l));
      return l(), o.promise(n)
    }
  });
  var z, X, U = /[\t\r\n\f]/g,
    V = /\r/g,
    Y = /^(?:input|select|textarea|button|object)$/i,
    J = /^(?:a|area)$/i,
    G = /^(?:checked|selected)$/i,
    Q = x.support.getSetAttribute,
    K = x.support.input;
  x.fn.extend({
    attr: function (e, t) {
      return x.access(this, x.attr, e, t, arguments.length > 1)
    },
    removeAttr: function (e) {
      return this.each(function () {
        x.removeAttr(this, e)
      })
    },
    prop: function (e, t) {
      return x.access(this, x.prop, e, t, arguments.length > 1)
    },
    removeProp: function (e) {
      return e = x.propFix[e] || e, this.each(function () {
        try {
          this[e] = t, delete this[e]
        } catch (n) {}
      })
    },
    addClass: function (e) {
      var t, n, r, i, o, a = 0,
        s = this.length,
        l = "string" == typeof e && e;
      if (x.isFunction(e)) return this.each(function (t) {
        x(this).addClass(e.call(this, t, this.className))
      });
      if (l)
        for (t = (e || "").match(T) || []; s > a; a++)
          if (n = this[a], r = 1 === n.nodeType && (n.className ? (" " + n.className + " ").replace(U, " ") : " ")) {
            o = 0;
            while (i = t[o++]) 0 > r.indexOf(" " + i + " ") && (r += i + " ");
            n.className = x.trim(r)
          }
      return this
    },
    removeClass: function (e) {
      var t, n, r, i, o, a = 0,
        s = this.length,
        l = 0 === arguments.length || "string" == typeof e && e;
      if (x.isFunction(e)) return this.each(function (t) {
        x(this).removeClass(e.call(this, t, this.className))
      });
      if (l)
        for (t = (e || "").match(T) || []; s > a; a++)
          if (n = this[a], r = 1 === n.nodeType && (n.className ? (" " + n.className + " ").replace(U, " ") : "")) {
            o = 0;
            while (i = t[o++])
              while (r.indexOf(" " + i + " ") >= 0) r = r.replace(" " + i + " ", " ");
            n.className = e ? x.trim(r) : ""
          }
      return this
    },
    toggleClass: function (e, t) {
      var n = typeof e;
      return "boolean" == typeof t && "string" === n ? t ? this.addClass(e) : this.removeClass(e) : x.isFunction(e) ? this.each(function (n) {
        x(this).toggleClass(e.call(this, n, this.className, t), t)
      }) : this.each(function () {
        if ("string" === n) {
          var t, r = 0,
            o = x(this),
            a = e.match(T) || [];
          while (t = a[r++]) o.hasClass(t) ? o.removeClass(t) : o.addClass(t)
        } else(n === i || "boolean" === n) && (this.className && x._data(this, "__className__", this.className), this.className = this.className || e === !1 ? "" : x._data(this, "__className__") || "")
      })
    },
    hasClass: function (e) {
      var t = " " + e + " ",
        n = 0,
        r = this.length;
      for (; r > n; n++)
        if (1 === this[n].nodeType && (" " + this[n].className + " ").replace(U, " ").indexOf(t) >= 0) return !0;
      return !1
    },
    val: function (e) {
      var n, r, i, o = this[0]; {
        if (arguments.length) return i = x.isFunction(e), this.each(function (n) {
          var o;
          1 === this.nodeType && (o = i ? e.call(this, n, x(this).val()) : e, null == o ? o = "" : "number" == typeof o ? o += "" : x.isArray(o) && (o = x.map(o, function (e) {
            return null == e ? "" : e + ""
          })), r = x.valHooks[this.type] || x.valHooks[this.nodeName.toLowerCase()], r && "set" in r && r.set(this, o, "value") !== t || (this.value = o))
        });
        if (o) return r = x.valHooks[o.type] || x.valHooks[o.nodeName.toLowerCase()], r && "get" in r && (n = r.get(o, "value")) !== t ? n : (n = o.value, "string" == typeof n ? n.replace(V, "") : null == n ? "" : n)
      }
    }
  }), x.extend({
    valHooks: {
      option: {
        get: function (e) {
          var t = x.find.attr(e, "value");
          return null != t ? t : e.text
        }
      },
      select: {
        get: function (e) {
          var t, n, r = e.options,
            i = e.selectedIndex,
            o = "select-one" === e.type || 0 > i,
            a = o ? null : [],
            s = o ? i + 1 : r.length,
            l = 0 > i ? s : o ? i : 0;
          for (; s > l; l++)
            if (n = r[l], !(!n.selected && l !== i || (x.support.optDisabled ? n.disabled : null !== n.getAttribute("disabled")) || n.parentNode.disabled && x.nodeName(n.parentNode, "optgroup"))) {
              if (t = x(n).val(), o) return t;
              a.push(t)
            }
          return a
        },
        set: function (e, t) {
          var n, r, i = e.options,
            o = x.makeArray(t),
            a = i.length;
          while (a--) r = i[a], (r.selected = x.inArray(x(r).val(), o) >= 0) && (n = !0);
          return n || (e.selectedIndex = -1), o
        }
      }
    },
    attr: function (e, n, r) {
      var o, a, s = e.nodeType;
      if (e && 3 !== s && 8 !== s && 2 !== s) return typeof e.getAttribute === i ? x.prop(e, n, r) : (1 === s && x.isXMLDoc(e) || (n = n.toLowerCase(), o = x.attrHooks[n] || (x.expr.match.bool.test(n) ? X : z)), r === t ? o && "get" in o && null !== (a = o.get(e, n)) ? a : (a = x.find.attr(e, n), null == a ? t : a) : null !== r ? o && "set" in o && (a = o.set(e, r, n)) !== t ? a : (e.setAttribute(n, r + ""), r) : (x.removeAttr(e, n), t))
    },
    removeAttr: function (e, t) {
      var n, r, i = 0,
        o = t && t.match(T);
      if (o && 1 === e.nodeType)
        while (n = o[i++]) r = x.propFix[n] || n, x.expr.match.bool.test(n) ? K && Q || !G.test(n) ? e[r] = !1 : e[x.camelCase("default-" + n)] = e[r] = !1 : x.attr(e, n, ""), e.removeAttribute(Q ? n : r)
    },
    attrHooks: {
      type: {
        set: function (e, t) {
          if (!x.support.radioValue && "radio" === t && x.nodeName(e, "input")) {
            var n = e.value;
            return e.setAttribute("type", t), n && (e.value = n), t
          }
        }
      }
    },
    propFix: {
      "for": "htmlFor",
      "class": "className"
    },
    prop: function (e, n, r) {
      var i, o, a, s = e.nodeType;
      if (e && 3 !== s && 8 !== s && 2 !== s) return a = 1 !== s || !x.isXMLDoc(e), a && (n = x.propFix[n] || n, o = x.propHooks[n]), r !== t ? o && "set" in o && (i = o.set(e, r, n)) !== t ? i : e[n] = r : o && "get" in o && null !== (i = o.get(e, n)) ? i : e[n]
    },
    propHooks: {
      tabIndex: {
        get: function (e) {
          var t = x.find.attr(e, "tabindex");
          return t ? parseInt(t, 10) : Y.test(e.nodeName) || J.test(e.nodeName) && e.href ? 0 : -1
        }
      }
    }
  }), X = {
    set: function (e, t, n) {
      return t === !1 ? x.removeAttr(e, n) : K && Q || !G.test(n) ? e.setAttribute(!Q && x.propFix[n] || n, n) : e[x.camelCase("default-" + n)] = e[n] = !0, n
    }
  }, x.each(x.expr.match.bool.source.match(/\w+/g), function (e, n) {
    var r = x.expr.attrHandle[n] || x.find.attr;
    x.expr.attrHandle[n] = K && Q || !G.test(n) ? function (e, n, i) {
      var o = x.expr.attrHandle[n],
        a = i ? t : (x.expr.attrHandle[n] = t) != r(e, n, i) ? n.toLowerCase() : null;
      return x.expr.attrHandle[n] = o, a
    } : function (e, n, r) {
      return r ? t : e[x.camelCase("default-" + n)] ? n.toLowerCase() : null
    }
  }), K && Q || (x.attrHooks.value = {
    set: function (e, n, r) {
      return x.nodeName(e, "input") ? (e.defaultValue = n, t) : z && z.set(e, n, r)
    }
  }), Q || (z = {
    set: function (e, n, r) {
      var i = e.getAttributeNode(r);
      return i || e.setAttributeNode(i = e.ownerDocument.createAttribute(r)), i.value = n += "", "value" === r || n === e.getAttribute(r) ? n : t
    }
  }, x.expr.attrHandle.id = x.expr.attrHandle.name = x.expr.attrHandle.coords = function (e, n, r) {
    var i;
    return r ? t : (i = e.getAttributeNode(n)) && "" !== i.value ? i.value : null
  }, x.valHooks.button = {
    get: function (e, n) {
      var r = e.getAttributeNode(n);
      return r && r.specified ? r.value : t
    },
    set: z.set
  }, x.attrHooks.contenteditable = {
    set: function (e, t, n) {
      z.set(e, "" === t ? !1 : t, n)
    }
  }, x.each(["width", "height"], function (e, n) {
    x.attrHooks[n] = {
      set: function (e, r) {
        return "" === r ? (e.setAttribute(n, "auto"), r) : t
      }
    }
  })), x.support.hrefNormalized || x.each(["href", "src"], function (e, t) {
    x.propHooks[t] = {
      get: function (e) {
        return e.getAttribute(t, 4)
      }
    }
  }), x.support.style || (x.attrHooks.style = {
    get: function (e) {
      return e.style.cssText || t
    },
    set: function (e, t) {
      return e.style.cssText = t + ""
    }
  }), x.support.optSelected || (x.propHooks.selected = {
    get: function (e) {
      var t = e.parentNode;
      return t && (t.selectedIndex, t.parentNode && t.parentNode.selectedIndex), null
    }
  }), x.each(["tabIndex", "readOnly", "maxLength", "cellSpacing", "cellPadding", "rowSpan", "colSpan", "useMap", "frameBorder", "contentEditable"], function () {
    x.propFix[this.toLowerCase()] = this
  }), x.support.enctype || (x.propFix.enctype = "encoding"), x.each(["radio", "checkbox"], function () {
    x.valHooks[this] = {
      set: function (e, n) {
        return x.isArray(n) ? e.checked = x.inArray(x(e).val(), n) >= 0 : t
      }
    }, x.support.checkOn || (x.valHooks[this].get = function (e) {
      return null === e.getAttribute("value") ? "on" : e.value
    })
  });
  var Z = /^(?:input|select|textarea)$/i,
    et = /^key/,
    tt = /^(?:mouse|contextmenu)|click/,
    nt = /^(?:focusinfocus|focusoutblur)$/,
    rt = /^([^.]*)(?:\.(.+)|)$/;

  function it() {
    return !0
  }

  function ot() {
    return !1
  }

  function at() {
    try {
      return a.activeElement
    } catch (e) {}
  }
  x.event = {
    global: {},
    add: function (e, n, r, o, a) {
      var s, l, u, c, p, f, d, h, g, m, y, v = x._data(e);
      if (v) {
        r.handler && (c = r, r = c.handler, a = c.selector), r.guid || (r.guid = x.guid++), (l = v.events) || (l = v.events = {}), (f = v.handle) || (f = v.handle = function (e) {
          return typeof x === i || e && x.event.triggered === e.type ? t : x.event.dispatch.apply(f.elem, arguments)
        }, f.elem = e), n = (n || "").match(T) || [""], u = n.length;
        while (u--) s = rt.exec(n[u]) || [], g = y = s[1], m = (s[2] || "").split(".").sort(), g && (p = x.event.special[g] || {}, g = (a ? p.delegateType : p.bindType) || g, p = x.event.special[g] || {}, d = x.extend({
          type: g,
          origType: y,
          data: o,
          handler: r,
          guid: r.guid,
          selector: a,
          needsContext: a && x.expr.match.needsContext.test(a),
          namespace: m.join(".")
        }, c), (h = l[g]) || (h = l[g] = [], h.delegateCount = 0, p.setup && p.setup.call(e, o, m, f) !== !1 || (e.addEventListener ? e.addEventListener(g, f, !1) : e.attachEvent && e.attachEvent("on" + g, f))), p.add && (p.add.call(e, d), d.handler.guid || (d.handler.guid = r.guid)), a ? h.splice(h.delegateCount++, 0, d) : h.push(d), x.event.global[g] = !0);
        e = null
      }
    },
    remove: function (e, t, n, r, i) {
      var o, a, s, l, u, c, p, f, d, h, g, m = x.hasData(e) && x._data(e);
      if (m && (c = m.events)) {
        t = (t || "").match(T) || [""], u = t.length;
        while (u--)
          if (s = rt.exec(t[u]) || [], d = g = s[1], h = (s[2] || "").split(".").sort(), d) {
            p = x.event.special[d] || {}, d = (r ? p.delegateType : p.bindType) || d, f = c[d] || [], s = s[2] && RegExp("(^|\\.)" + h.join("\\.(?:.*\\.|)") + "(\\.|$)"), l = o = f.length;
            while (o--) a = f[o], !i && g !== a.origType || n && n.guid !== a.guid || s && !s.test(a.namespace) || r && r !== a.selector && ("**" !== r || !a.selector) || (f.splice(o, 1), a.selector && f.delegateCount--, p.remove && p.remove.call(e, a));
            l && !f.length && (p.teardown && p.teardown.call(e, h, m.handle) !== !1 || x.removeEvent(e, d, m.handle), delete c[d])
          } else
            for (d in c) x.event.remove(e, d + t[u], n, r, !0);
        x.isEmptyObject(c) && (delete m.handle, x._removeData(e, "events"))
      }
    },
    trigger: function (n, r, i, o) {
      var s, l, u, c, p, f, d, h = [i || a],
        g = v.call(n, "type") ? n.type : n,
        m = v.call(n, "namespace") ? n.namespace.split(".") : [];
      if (u = f = i = i || a, 3 !== i.nodeType && 8 !== i.nodeType && !nt.test(g + x.event.triggered) && (g.indexOf(".") >= 0 && (m = g.split("."), g = m.shift(), m.sort()), l = 0 > g.indexOf(":") && "on" + g, n = n[x.expando] ? n : new x.Event(g, "object" == typeof n && n), n.isTrigger = o ? 2 : 3, n.namespace = m.join("."), n.namespace_re = n.namespace ? RegExp("(^|\\.)" + m.join("\\.(?:.*\\.|)") + "(\\.|$)") : null, n.result = t, n.target || (n.target = i), r = null == r ? [n] : x.makeArray(r, [n]), p = x.event.special[g] || {}, o || !p.trigger || p.trigger.apply(i, r) !== !1)) {
        if (!o && !p.noBubble && !x.isWindow(i)) {
          for (c = p.delegateType || g, nt.test(c + g) || (u = u.parentNode); u; u = u.parentNode) h.push(u), f = u;
          f === (i.ownerDocument || a) && h.push(f.defaultView || f.parentWindow || e)
        }
        d = 0;
        while ((u = h[d++]) && !n.isPropagationStopped()) n.type = d > 1 ? c : p.bindType || g, s = (x._data(u, "events") || {})[n.type] && x._data(u, "handle"), s && s.apply(u, r), s = l && u[l], s && x.acceptData(u) && s.apply && s.apply(u, r) === !1 && n.preventDefault();
        if (n.type = g, !o && !n.isDefaultPrevented() && (!p._default || p._default.apply(h.pop(), r) === !1) && x.acceptData(i) && l && i[g] && !x.isWindow(i)) {
          f = i[l], f && (i[l] = null), x.event.triggered = g;
          try {
            i[g]()
          } catch (y) {}
          x.event.triggered = t, f && (i[l] = f)
        }
        return n.result
      }
    },
    dispatch: function (e) {
      e = x.event.fix(e);
      var n, r, i, o, a, s = [],
        l = g.call(arguments),
        u = (x._data(this, "events") || {})[e.type] || [],
        c = x.event.special[e.type] || {};
      if (l[0] = e, e.delegateTarget = this, !c.preDispatch || c.preDispatch.call(this, e) !== !1) {
        s = x.event.handlers.call(this, e, u), n = 0;
        while ((o = s[n++]) && !e.isPropagationStopped()) {
          e.currentTarget = o.elem, a = 0;
          while ((i = o.handlers[a++]) && !e.isImmediatePropagationStopped())(!e.namespace_re || e.namespace_re.test(i.namespace)) && (e.handleObj = i, e.data = i.data, r = ((x.event.special[i.origType] || {}).handle || i.handler).apply(o.elem, l), r !== t && (e.result = r) === !1 && (e.preventDefault(), e.stopPropagation()))
        }
        return c.postDispatch && c.postDispatch.call(this, e), e.result
      }
    },
    handlers: function (e, n) {
      var r, i, o, a, s = [],
        l = n.delegateCount,
        u = e.target;
      if (l && u.nodeType && (!e.button || "click" !== e.type))
        for (; u != this; u = u.parentNode || this)
          if (1 === u.nodeType && (u.disabled !== !0 || "click" !== e.type)) {
            for (o = [], a = 0; l > a; a++) i = n[a], r = i.selector + " ", o[r] === t && (o[r] = i.needsContext ? x(r, this).index(u) >= 0 : x.find(r, this, null, [u]).length), o[r] && o.push(i);
            o.length && s.push({
              elem: u,
              handlers: o
            })
          }
      return n.length > l && s.push({
        elem: this,
        handlers: n.slice(l)
      }), s
    },
    fix: function (e) {
      if (e[x.expando]) return e;
      var t, n, r, i = e.type,
        o = e,
        s = this.fixHooks[i];
      s || (this.fixHooks[i] = s = tt.test(i) ? this.mouseHooks : et.test(i) ? this.keyHooks : {}), r = s.props ? this.props.concat(s.props) : this.props, e = new x.Event(o), t = r.length;
      while (t--) n = r[t], e[n] = o[n];
      return e.target || (e.target = o.srcElement || a), 3 === e.target.nodeType && (e.target = e.target.parentNode), e.metaKey = !!e.metaKey, s.filter ? s.filter(e, o) : e
    },
    props: "altKey bubbles cancelable ctrlKey currentTarget eventPhase metaKey relatedTarget shiftKey target timeStamp view which".split(" "),
    fixHooks: {},
    keyHooks: {
      props: "char charCode key keyCode".split(" "),
      filter: function (e, t) {
        return null == e.which && (e.which = null != t.charCode ? t.charCode : t.keyCode), e
      }
    },
    mouseHooks: {
      props: "button buttons clientX clientY fromElement offsetX offsetY pageX pageY screenX screenY toElement".split(" "),
      filter: function (e, n) {
        var r, i, o, s = n.button,
          l = n.fromElement;
        return null == e.pageX && null != n.clientX && (i = e.target.ownerDocument || a, o = i.documentElement, r = i.body, e.pageX = n.clientX + (o && o.scrollLeft || r && r.scrollLeft || 0) - (o && o.clientLeft || r && r.clientLeft || 0), e.pageY = n.clientY + (o && o.scrollTop || r && r.scrollTop || 0) - (o && o.clientTop || r && r.clientTop || 0)), !e.relatedTarget && l && (e.relatedTarget = l === e.target ? n.toElement : l), e.which || s === t || (e.which = 1 & s ? 1 : 2 & s ? 3 : 4 & s ? 2 : 0), e
      }
    },
    special: {
      load: {
        noBubble: !0
      },
      focus: {
        trigger: function () {
          if (this !== at() && this.focus) try {
            return this.focus(), !1
          } catch (e) {}
        },
        delegateType: "focusin"
      },
      blur: {
        trigger: function () {
          return this === at() && this.blur ? (this.blur(), !1) : t
        },
        delegateType: "focusout"
      },
      click: {
        trigger: function () {
          return x.nodeName(this, "input") && "checkbox" === this.type && this.click ? (this.click(), !1) : t
        },
        _default: function (e) {
          return x.nodeName(e.target, "a")
        }
      },
      beforeunload: {
        postDispatch: function (e) {
          e.result !== t && (e.originalEvent.returnValue = e.result)
        }
      }
    },
    simulate: function (e, t, n, r) {
      var i = x.extend(new x.Event, n, {
        type: e,
        isSimulated: !0,
        originalEvent: {}
      });
      r ? x.event.trigger(i, null, t) : x.event.dispatch.call(t, i), i.isDefaultPrevented() && n.preventDefault()
    }
  }, x.removeEvent = a.removeEventListener ? function (e, t, n) {
    e.removeEventListener && e.removeEventListener(t, n, !1)
  } : function (e, t, n) {
    var r = "on" + t;
    e.detachEvent && (typeof e[r] === i && (e[r] = null), e.detachEvent(r, n))
  }, x.Event = function (e, n) {
    return this instanceof x.Event ? (e && e.type ? (this.originalEvent = e, this.type = e.type, this.isDefaultPrevented = e.defaultPrevented || e.returnValue === !1 || e.getPreventDefault && e.getPreventDefault() ? it : ot) : this.type = e, n && x.extend(this, n), this.timeStamp = e && e.timeStamp || x.now(), this[x.expando] = !0, t) : new x.Event(e, n)
  }, x.Event.prototype = {
    isDefaultPrevented: ot,
    isPropagationStopped: ot,
    isImmediatePropagationStopped: ot,
    preventDefault: function () {
      var e = this.originalEvent;
      this.isDefaultPrevented = it, e && (e.preventDefault ? e.preventDefault() : e.returnValue = !1)
    },
    stopPropagation: function () {
      var e = this.originalEvent;
      this.isPropagationStopped = it, e && (e.stopPropagation && e.stopPropagation(), e.cancelBubble = !0)
    },
    stopImmediatePropagation: function () {
      this.isImmediatePropagationStopped = it, this.stopPropagation()
    }
  }, x.each({
    mouseenter: "mouseover",
    mouseleave: "mouseout"
  }, function (e, t) {
    x.event.special[e] = {
      delegateType: t,
      bindType: t,
      handle: function (e) {
        var n, r = this,
          i = e.relatedTarget,
          o = e.handleObj;
        return (!i || i !== r && !x.contains(r, i)) && (e.type = o.origType, n = o.handler.apply(this, arguments), e.type = t), n
      }
    }
  }), x.support.submitBubbles || (x.event.special.submit = {
    setup: function () {
      return x.nodeName(this, "form") ? !1 : (x.event.add(this, "click._submit keypress._submit", function (e) {
        var n = e.target,
          r = x.nodeName(n, "input") || x.nodeName(n, "button") ? n.form : t;
        r && !x._data(r, "submitBubbles") && (x.event.add(r, "submit._submit", function (e) {
          e._submit_bubble = !0
        }), x._data(r, "submitBubbles", !0))
      }), t)
    },
    postDispatch: function (e) {
      e._submit_bubble && (delete e._submit_bubble, this.parentNode && !e.isTrigger && x.event.simulate("submit", this.parentNode, e, !0))
    },
    teardown: function () {
      return x.nodeName(this, "form") ? !1 : (x.event.remove(this, "._submit"), t)
    }
  }), x.support.changeBubbles || (x.event.special.change = {
    setup: function () {
      return Z.test(this.nodeName) ? (("checkbox" === this.type || "radio" === this.type) && (x.event.add(this, "propertychange._change", function (e) {
        "checked" === e.originalEvent.propertyName && (this._just_changed = !0)
      }), x.event.add(this, "click._change", function (e) {
        this._just_changed && !e.isTrigger && (this._just_changed = !1), x.event.simulate("change", this, e, !0)
      })), !1) : (x.event.add(this, "beforeactivate._change", function (e) {
        var t = e.target;
        Z.test(t.nodeName) && !x._data(t, "changeBubbles") && (x.event.add(t, "change._change", function (e) {
          !this.parentNode || e.isSimulated || e.isTrigger || x.event.simulate("change", this.parentNode, e, !0)
        }), x._data(t, "changeBubbles", !0))
      }), t)
    },
    handle: function (e) {
      var n = e.target;
      return this !== n || e.isSimulated || e.isTrigger || "radio" !== n.type && "checkbox" !== n.type ? e.handleObj.handler.apply(this, arguments) : t
    },
    teardown: function () {
      return x.event.remove(this, "._change"), !Z.test(this.nodeName)
    }
  }), x.support.focusinBubbles || x.each({
    focus: "focusin",
    blur: "focusout"
  }, function (e, t) {
    var n = 0,
      r = function (e) {
        x.event.simulate(t, e.target, x.event.fix(e), !0)
      };
    x.event.special[t] = {
      setup: function () {
        0 === n++ && a.addEventListener(e, r, !0)
      },
      teardown: function () {
        0 === --n && a.removeEventListener(e, r, !0)
      }
    }
  }), x.fn.extend({
    on: function (e, n, r, i, o) {
      var a, s;
      if ("object" == typeof e) {
        "string" != typeof n && (r = r || n, n = t);
        for (a in e) this.on(a, n, r, e[a], o);
        return this
      }
      if (null == r && null == i ? (i = n, r = n = t) : null == i && ("string" == typeof n ? (i = r, r = t) : (i = r, r = n, n = t)), i === !1) i = ot;
      else if (!i) return this;
      return 1 === o && (s = i, i = function (e) {
        return x().off(e), s.apply(this, arguments)
      }, i.guid = s.guid || (s.guid = x.guid++)), this.each(function () {
        x.event.add(this, e, i, r, n)
      })
    },
    one: function (e, t, n, r) {
      return this.on(e, t, n, r, 1)
    },
    off: function (e, n, r) {
      var i, o;
      if (e && e.preventDefault && e.handleObj) return i = e.handleObj, x(e.delegateTarget).off(i.namespace ? i.origType + "." + i.namespace : i.origType, i.selector, i.handler), this;
      if ("object" == typeof e) {
        for (o in e) this.off(o, n, e[o]);
        return this
      }
      return (n === !1 || "function" == typeof n) && (r = n, n = t), r === !1 && (r = ot), this.each(function () {
        x.event.remove(this, e, r, n)
      })
    },
    trigger: function (e, t) {
      return this.each(function () {
        x.event.trigger(e, t, this)
      })
    },
    triggerHandler: function (e, n) {
      var r = this[0];
      return r ? x.event.trigger(e, n, r, !0) : t
    }
  });
  var st = /^.[^:#\[\.,]*$/,
    lt = /^(?:parents|prev(?:Until|All))/,
    ut = x.expr.match.needsContext,
    ct = {
      children: !0,
      contents: !0,
      next: !0,
      prev: !0
    };
  x.fn.extend({
    find: function (e) {
      var t, n = [],
        r = this,
        i = r.length;
      if ("string" != typeof e) return this.pushStack(x(e).filter(function () {
        for (t = 0; i > t; t++)
          if (x.contains(r[t], this)) return !0
      }));
      for (t = 0; i > t; t++) x.find(e, r[t], n);
      return n = this.pushStack(i > 1 ? x.unique(n) : n), n.selector = this.selector ? this.selector + " " + e : e, n
    },
    has: function (e) {
      var t, n = x(e, this),
        r = n.length;
      return this.filter(function () {
        for (t = 0; r > t; t++)
          if (x.contains(this, n[t])) return !0
      })
    },
    not: function (e) {
      return this.pushStack(ft(this, e || [], !0))
    },
    filter: function (e) {
      return this.pushStack(ft(this, e || [], !1))
    },
    is: function (e) {
      return !!ft(this, "string" == typeof e && ut.test(e) ? x(e) : e || [], !1).length
    },
    closest: function (e, t) {
      var n, r = 0,
        i = this.length,
        o = [],
        a = ut.test(e) || "string" != typeof e ? x(e, t || this.context) : 0;
      for (; i > r; r++)
        for (n = this[r]; n && n !== t; n = n.parentNode)
          if (11 > n.nodeType && (a ? a.index(n) > -1 : 1 === n.nodeType && x.find.matchesSelector(n, e))) {
            n = o.push(n);
            break
          }
      return this.pushStack(o.length > 1 ? x.unique(o) : o)
    },
    index: function (e) {
      return e ? "string" == typeof e ? x.inArray(this[0], x(e)) : x.inArray(e.jquery ? e[0] : e, this) : this[0] && this[0].parentNode ? this.first().prevAll().length : -1
    },
    add: function (e, t) {
      var n = "string" == typeof e ? x(e, t) : x.makeArray(e && e.nodeType ? [e] : e),
        r = x.merge(this.get(), n);
      return this.pushStack(x.unique(r))
    },
    addBack: function (e) {
      return this.add(null == e ? this.prevObject : this.prevObject.filter(e))
    }
  });

  function pt(e, t) {
    do e = e[t]; while (e && 1 !== e.nodeType);
    return e
  }
  x.each({
    parent: function (e) {
      var t = e.parentNode;
      return t && 11 !== t.nodeType ? t : null
    },
    parents: function (e) {
      return x.dir(e, "parentNode")
    },
    parentsUntil: function (e, t, n) {
      return x.dir(e, "parentNode", n)
    },
    next: function (e) {
      return pt(e, "nextSibling")
    },
    prev: function (e) {
      return pt(e, "previousSibling")
    },
    nextAll: function (e) {
      return x.dir(e, "nextSibling")
    },
    prevAll: function (e) {
      return x.dir(e, "previousSibling")
    },
    nextUntil: function (e, t, n) {
      return x.dir(e, "nextSibling", n)
    },
    prevUntil: function (e, t, n) {
      return x.dir(e, "previousSibling", n)
    },
    siblings: function (e) {
      return x.sibling((e.parentNode || {}).firstChild, e)
    },
    children: function (e) {
      return x.sibling(e.firstChild)
    },
    contents: function (e) {
      return x.nodeName(e, "iframe") ? e.contentDocument || e.contentWindow.document : x.merge([], e.childNodes)
    }
  }, function (e, t) {
    x.fn[e] = function (n, r) {
      var i = x.map(this, t, n);
      return "Until" !== e.slice(-5) && (r = n), r && "string" == typeof r && (i = x.filter(r, i)), this.length > 1 && (ct[e] || (i = x.unique(i)), lt.test(e) && (i = i.reverse())), this.pushStack(i)
    }
  }), x.extend({
    filter: function (e, t, n) {
      var r = t[0];
      return n && (e = ":not(" + e + ")"), 1 === t.length && 1 === r.nodeType ? x.find.matchesSelector(r, e) ? [r] : [] : x.find.matches(e, x.grep(t, function (e) {
        return 1 === e.nodeType
      }))
    },
    dir: function (e, n, r) {
      var i = [],
        o = e[n];
      while (o && 9 !== o.nodeType && (r === t || 1 !== o.nodeType || !x(o).is(r))) 1 === o.nodeType && i.push(o), o = o[n];
      return i
    },
    sibling: function (e, t) {
      var n = [];
      for (; e; e = e.nextSibling) 1 === e.nodeType && e !== t && n.push(e);
      return n
    }
  });

  function ft(e, t, n) {
    if (x.isFunction(t)) return x.grep(e, function (e, r) {
      return !!t.call(e, r, e) !== n
    });
    if (t.nodeType) return x.grep(e, function (e) {
      return e === t !== n
    });
    if ("string" == typeof t) {
      if (st.test(t)) return x.filter(t, e, n);
      t = x.filter(t, e)
    }
    return x.grep(e, function (e) {
      return x.inArray(e, t) >= 0 !== n
    })
  }

  function dt(e) {
    var t = ht.split("|"),
      n = e.createDocumentFragment();
    if (n.createElement)
      while (t.length) n.createElement(t.pop());
    return n
  }
  var ht = "abbr|article|aside|audio|bdi|canvas|data|datalist|details|figcaption|figure|footer|header|hgroup|mark|meter|nav|output|progress|section|summary|time|video",
    gt = / jQuery\d+="(?:null|\d+)"/g,
    mt = RegExp("<(?:" + ht + ")[\\s/>]", "i"),
    yt = /^\s+/,
    vt = /<(?!area|br|col|embed|hr|img|input|link|meta|param)(([\w:]+)[^>]*)\/>/gi,
    bt = /<([\w:]+)/,
    xt = /<tbody/i,
    wt = /<|&#?\w+;/,
    Tt = /<(?:script|style|link)/i,
    Ct = /^(?:checkbox|radio)$/i,
    Nt = /checked\s*(?:[^=]|=\s*.checked.)/i,
    kt = /^$|\/(?:java|ecma)script/i,
    Et = /^true\/(.*)/,
    St = /^\s*<!(?:\[CDATA\[|--)|(?:\]\]|--)>\s*$/g,
    At = {
      option: [1, "<select multiple='multiple'>", "</select>"],
      legend: [1, "<fieldset>", "</fieldset>"],
      area: [1, "<map>", "</map>"],
      param: [1, "<object>", "</object>"],
      thead: [1, "<table>", "</table>"],
      tr: [2, "<table><tbody>", "</tbody></table>"],
      col: [2, "<table><tbody></tbody><colgroup>", "</colgroup></table>"],
      td: [3, "<table><tbody><tr>", "</tr></tbody></table>"],
      _default: x.support.htmlSerialize ? [0, "", ""] : [1, "X<div>", "</div>"]
    },
    jt = dt(a),
    Dt = jt.appendChild(a.createElement("div"));
  At.optgroup = At.option, At.tbody = At.tfoot = At.colgroup = At.caption = At.thead, At.th = At.td, x.fn.extend({
    text: function (e) {
      return x.access(this, function (e) {
        return e === t ? x.text(this) : this.empty().append((this[0] && this[0].ownerDocument || a).createTextNode(e))
      }, null, e, arguments.length)
    },
    append: function () {
      return this.domManip(arguments, function (e) {
        if (1 === this.nodeType || 11 === this.nodeType || 9 === this.nodeType) {
          var t = Lt(this, e);
          t.appendChild(e)
        }
      })
    },
    prepend: function () {
      return this.domManip(arguments, function (e) {
        if (1 === this.nodeType || 11 === this.nodeType || 9 === this.nodeType) {
          var t = Lt(this, e);
          t.insertBefore(e, t.firstChild)
        }
      })
    },
    before: function () {
      return this.domManip(arguments, function (e) {
        this.parentNode && this.parentNode.insertBefore(e, this)
      })
    },
    after: function () {
      return this.domManip(arguments, function (e) {
        this.parentNode && this.parentNode.insertBefore(e, this.nextSibling)
      })
    },
    remove: function (e, t) {
      var n, r = e ? x.filter(e, this) : this,
        i = 0;
      for (; null != (n = r[i]); i++) t || 1 !== n.nodeType || x.cleanData(Ft(n)), n.parentNode && (t && x.contains(n.ownerDocument, n) && _t(Ft(n, "script")), n.parentNode.removeChild(n));
      return this
    },
    empty: function () {
      var e, t = 0;
      for (; null != (e = this[t]); t++) {
        1 === e.nodeType && x.cleanData(Ft(e, !1));
        while (e.firstChild) e.removeChild(e.firstChild);
        e.options && x.nodeName(e, "select") && (e.options.length = 0)
      }
      return this
    },
    clone: function (e, t) {
      return e = null == e ? !1 : e, t = null == t ? e : t, this.map(function () {
        return x.clone(this, e, t)
      })
    },
    html: function (e) {
      return x.access(this, function (e) {
        var n = this[0] || {},
          r = 0,
          i = this.length;
        if (e === t) return 1 === n.nodeType ? n.innerHTML.replace(gt, "") : t;
        if (!("string" != typeof e || Tt.test(e) || !x.support.htmlSerialize && mt.test(e) || !x.support.leadingWhitespace && yt.test(e) || At[(bt.exec(e) || ["", ""])[1].toLowerCase()])) {
          e = e.replace(vt, "<$1></$2>");
          try {
            for (; i > r; r++) n = this[r] || {}, 1 === n.nodeType && (x.cleanData(Ft(n, !1)), n.innerHTML = e);
            n = 0
          } catch (o) {}
        }
        n && this.empty().append(e)
      }, null, e, arguments.length)
    },
    replaceWith: function () {
      var e = x.map(this, function (e) {
          return [e.nextSibling, e.parentNode]
        }),
        t = 0;
      return this.domManip(arguments, function (n) {
        var r = e[t++],
          i = e[t++];
        i && (r && r.parentNode !== i && (r = this.nextSibling), x(this).remove(), i.insertBefore(n, r))
      }, !0), t ? this : this.remove()
    },
    detach: function (e) {
      return this.remove(e, !0)
    },
    domManip: function (e, t, n) {
      e = d.apply([], e);
      var r, i, o, a, s, l, u = 0,
        c = this.length,
        p = this,
        f = c - 1,
        h = e[0],
        g = x.isFunction(h);
      if (g || !(1 >= c || "string" != typeof h || x.support.checkClone) && Nt.test(h)) return this.each(function (r) {
        var i = p.eq(r);
        g && (e[0] = h.call(this, r, i.html())), i.domManip(e, t, n)
      });
      if (c && (l = x.buildFragment(e, this[0].ownerDocument, !1, !n && this), r = l.firstChild, 1 === l.childNodes.length && (l = r), r)) {
        for (a = x.map(Ft(l, "script"), Ht), o = a.length; c > u; u++) i = l, u !== f && (i = x.clone(i, !0, !0), o && x.merge(a, Ft(i, "script"))), t.call(this[u], i, u);
        if (o)
          for (s = a[a.length - 1].ownerDocument, x.map(a, qt), u = 0; o > u; u++) i = a[u], kt.test(i.type || "") && !x._data(i, "globalEval") && x.contains(s, i) && (i.src ? x._evalUrl(i.src) : x.globalEval((i.text || i.textContent || i.innerHTML || "").replace(St, "")));
        l = r = null
      }
      return this
    }
  });

  function Lt(e, t) {
    return x.nodeName(e, "table") && x.nodeName(1 === t.nodeType ? t : t.firstChild, "tr") ? e.getElementsByTagName("tbody")[0] || e.appendChild(e.ownerDocument.createElement("tbody")) : e
  }

  function Ht(e) {
    return e.type = (null !== x.find.attr(e, "type")) + "/" + e.type, e
  }

  function qt(e) {
    var t = Et.exec(e.type);
    return t ? e.type = t[1] : e.removeAttribute("type"), e
  }

  function _t(e, t) {
    var n, r = 0;
    for (; null != (n = e[r]); r++) x._data(n, "globalEval", !t || x._data(t[r], "globalEval"))
  }

  function Mt(e, t) {
    if (1 === t.nodeType && x.hasData(e)) {
      var n, r, i, o = x._data(e),
        a = x._data(t, o),
        s = o.events;
      if (s) {
        delete a.handle, a.events = {};
        for (n in s)
          for (r = 0, i = s[n].length; i > r; r++) x.event.add(t, n, s[n][r])
      }
      a.data && (a.data = x.extend({}, a.data))
    }
  }

  function Ot(e, t) {
    var n, r, i;
    if (1 === t.nodeType) {
      if (n = t.nodeName.toLowerCase(), !x.support.noCloneEvent && t[x.expando]) {
        i = x._data(t);
        for (r in i.events) x.removeEvent(t, r, i.handle);
        t.removeAttribute(x.expando)
      }
      "script" === n && t.text !== e.text ? (Ht(t).text = e.text, qt(t)) : "object" === n ? (t.parentNode && (t.outerHTML = e.outerHTML), x.support.html5Clone && e.innerHTML && !x.trim(t.innerHTML) && (t.innerHTML = e.innerHTML)) : "input" === n && Ct.test(e.type) ? (t.defaultChecked = t.checked = e.checked, t.value !== e.value && (t.value = e.value)) : "option" === n ? t.defaultSelected = t.selected = e.defaultSelected : ("input" === n || "textarea" === n) && (t.defaultValue = e.defaultValue)
    }
  }
  x.each({
    appendTo: "append",
    prependTo: "prepend",
    insertBefore: "before",
    insertAfter: "after",
    replaceAll: "replaceWith"
  }, function (e, t) {
    x.fn[e] = function (e) {
      var n, r = 0,
        i = [],
        o = x(e),
        a = o.length - 1;
      for (; a >= r; r++) n = r === a ? this : this.clone(!0), x(o[r])[t](n), h.apply(i, n.get());
      return this.pushStack(i)
    }
  });

  function Ft(e, n) {
    var r, o, a = 0,
      s = typeof e.getElementsByTagName !== i ? e.getElementsByTagName(n || "*") : typeof e.querySelectorAll !== i ? e.querySelectorAll(n || "*") : t;
    if (!s)
      for (s = [], r = e.childNodes || e; null != (o = r[a]); a++) !n || x.nodeName(o, n) ? s.push(o) : x.merge(s, Ft(o, n));
    return n === t || n && x.nodeName(e, n) ? x.merge([e], s) : s
  }

  function Bt(e) {
    Ct.test(e.type) && (e.defaultChecked = e.checked)
  }
  x.extend({
    clone: function (e, t, n) {
      var r, i, o, a, s, l = x.contains(e.ownerDocument, e);
      if (x.support.html5Clone || x.isXMLDoc(e) || !mt.test("<" + e.nodeName + ">") ? o = e.cloneNode(!0) : (Dt.innerHTML = e.outerHTML, Dt.removeChild(o = Dt.firstChild)), !(x.support.noCloneEvent && x.support.noCloneChecked || 1 !== e.nodeType && 11 !== e.nodeType || x.isXMLDoc(e)))
        for (r = Ft(o), s = Ft(e), a = 0; null != (i = s[a]); ++a) r[a] && Ot(i, r[a]);
      if (t)
        if (n)
          for (s = s || Ft(e), r = r || Ft(o), a = 0; null != (i = s[a]); a++) Mt(i, r[a]);
        else Mt(e, o);
      return r = Ft(o, "script"), r.length > 0 && _t(r, !l && Ft(e, "script")), r = s = i = null, o
    },
    buildFragment: function (e, t, n, r) {
      var i, o, a, s, l, u, c, p = e.length,
        f = dt(t),
        d = [],
        h = 0;
      for (; p > h; h++)
        if (o = e[h], o || 0 === o)
          if ("object" === x.type(o)) x.merge(d, o.nodeType ? [o] : o);
          else if (wt.test(o)) {
        s = s || f.appendChild(t.createElement("div")), l = (bt.exec(o) || ["", ""])[1].toLowerCase(), c = At[l] || At._default, s.innerHTML = c[1] + o.replace(vt, "<$1></$2>") + c[2], i = c[0];
        while (i--) s = s.lastChild;
        if (!x.support.leadingWhitespace && yt.test(o) && d.push(t.createTextNode(yt.exec(o)[0])), !x.support.tbody) {
          o = "table" !== l || xt.test(o) ? "<table>" !== c[1] || xt.test(o) ? 0 : s : s.firstChild, i = o && o.childNodes.length;
          while (i--) x.nodeName(u = o.childNodes[i], "tbody") && !u.childNodes.length && o.removeChild(u)
        }
        x.merge(d, s.childNodes), s.textContent = "";
        while (s.firstChild) s.removeChild(s.firstChild);
        s = f.lastChild
      } else d.push(t.createTextNode(o));
      s && f.removeChild(s), x.support.appendChecked || x.grep(Ft(d, "input"), Bt), h = 0;
      while (o = d[h++])
        if ((!r || -1 === x.inArray(o, r)) && (a = x.contains(o.ownerDocument, o), s = Ft(f.appendChild(o), "script"), a && _t(s), n)) {
          i = 0;
          while (o = s[i++]) kt.test(o.type || "") && n.push(o)
        }
      return s = null, f
    },
    cleanData: function (e, t) {
      var n, r, o, a, s = 0,
        l = x.expando,
        u = x.cache,
        c = x.support.deleteExpando,
        f = x.event.special;
      for (; null != (n = e[s]); s++)
        if ((t || x.acceptData(n)) && (o = n[l], a = o && u[o])) {
          if (a.events)
            for (r in a.events) f[r] ? x.event.remove(n, r) : x.removeEvent(n, r, a.handle);
          u[o] && (delete u[o], c ? delete n[l] : typeof n.removeAttribute !== i ? n.removeAttribute(l) : n[l] = null, p.push(o))
        }
    },
    _evalUrl: function (e) {
      return x.ajax({
        url: e,
        type: "GET",
        dataType: "script",
        async: !1,
        global: !1,
        "throws": !0
      })
    }
  }), x.fn.extend({
    wrapAll: function (e) {
      if (x.isFunction(e)) return this.each(function (t) {
        x(this).wrapAll(e.call(this, t))
      });
      if (this[0]) {
        var t = x(e, this[0].ownerDocument).eq(0).clone(!0);
        this[0].parentNode && t.insertBefore(this[0]), t.map(function () {
          var e = this;
          while (e.firstChild && 1 === e.firstChild.nodeType) e = e.firstChild;
          return e
        }).append(this)
      }
      return this
    },
    wrapInner: function (e) {
      return x.isFunction(e) ? this.each(function (t) {
        x(this).wrapInner(e.call(this, t))
      }) : this.each(function () {
        var t = x(this),
          n = t.contents();
        n.length ? n.wrapAll(e) : t.append(e)
      })
    },
    wrap: function (e) {
      var t = x.isFunction(e);
      return this.each(function (n) {
        x(this).wrapAll(t ? e.call(this, n) : e)
      })
    },
    unwrap: function () {
      return this.parent().each(function () {
        x.nodeName(this, "body") || x(this).replaceWith(this.childNodes)
      }).end()
    }
  });
  var Pt, Rt, Wt, $t = /alpha\([^)]*\)/i,
    It = /opacity\s*=\s*([^)]*)/,
    zt = /^(top|right|bottom|left)$/,
    Xt = /^(none|table(?!-c[ea]).+)/,
    Ut = /^margin/,
    Vt = RegExp("^(" + w + ")(.*)$", "i"),
    Yt = RegExp("^(" + w + ")(?!px)[a-z%]+$", "i"),
    Jt = RegExp("^([+-])=(" + w + ")", "i"),
    Gt = {
      BODY: "block"
    },
    Qt = {
      position: "absolute",
      visibility: "hidden",
      display: "block"
    },
    Kt = {
      letterSpacing: 0,
      fontWeight: 400
    },
    Zt = ["Top", "Right", "Bottom", "Left"],
    en = ["Webkit", "O", "Moz", "ms"];

  function tn(e, t) {
    if (t in e) return t;
    var n = t.charAt(0).toUpperCase() + t.slice(1),
      r = t,
      i = en.length;
    while (i--)
      if (t = en[i] + n, t in e) return t;
    return r
  }

  function nn(e, t) {
    return e = t || e, "none" === x.css(e, "display") || !x.contains(e.ownerDocument, e)
  }

  function rn(e, t) {
    var n, r, i, o = [],
      a = 0,
      s = e.length;
    for (; s > a; a++) r = e[a], r.style && (o[a] = x._data(r, "olddisplay"), n = r.style.display, t ? (o[a] || "none" !== n || (r.style.display = ""), "" === r.style.display && nn(r) && (o[a] = x._data(r, "olddisplay", ln(r.nodeName)))) : o[a] || (i = nn(r), (n && "none" !== n || !i) && x._data(r, "olddisplay", i ? n : x.css(r, "display"))));
    for (a = 0; s > a; a++) r = e[a], r.style && (t && "none" !== r.style.display && "" !== r.style.display || (r.style.display = t ? o[a] || "" : "none"));
    return e
  }
  x.fn.extend({
    css: function (e, n) {
      return x.access(this, function (e, n, r) {
        var i, o, a = {},
          s = 0;
        if (x.isArray(n)) {
          for (o = Rt(e), i = n.length; i > s; s++) a[n[s]] = x.css(e, n[s], !1, o);
          return a
        }
        return r !== t ? x.style(e, n, r) : x.css(e, n)
      }, e, n, arguments.length > 1)
    },
    show: function () {
      return rn(this, !0)
    },
    hide: function () {
      return rn(this)
    },
    toggle: function (e) {
      return "boolean" == typeof e ? e ? this.show() : this.hide() : this.each(function () {
        nn(this) ? x(this).show() : x(this).hide()
      })
    }
  }), x.extend({
    cssHooks: {
      opacity: {
        get: function (e, t) {
          if (t) {
            var n = Wt(e, "opacity");
            return "" === n ? "1" : n
          }
        }
      }
    },
    cssNumber: {
      columnCount: !0,
      fillOpacity: !0,
      fontWeight: !0,
      lineHeight: !0,
      opacity: !0,
      order: !0,
      orphans: !0,
      widows: !0,
      zIndex: !0,
      zoom: !0
    },
    cssProps: {
      "float": x.support.cssFloat ? "cssFloat" : "styleFloat"
    },
    style: function (e, n, r, i) {
      if (e && 3 !== e.nodeType && 8 !== e.nodeType && e.style) {
        var o, a, s, l = x.camelCase(n),
          u = e.style;
        if (n = x.cssProps[l] || (x.cssProps[l] = tn(u, l)), s = x.cssHooks[n] || x.cssHooks[l], r === t) return s && "get" in s && (o = s.get(e, !1, i)) !== t ? o : u[n];
        if (a = typeof r, "string" === a && (o = Jt.exec(r)) && (r = (o[1] + 1) * o[2] + parseFloat(x.css(e, n)), a = "number"), !(null == r || "number" === a && isNaN(r) || ("number" !== a || x.cssNumber[l] || (r += "px"), x.support.clearCloneStyle || "" !== r || 0 !== n.indexOf("background") || (u[n] = "inherit"), s && "set" in s && (r = s.set(e, r, i)) === t))) try {
          u[n] = r
        } catch (c) {}
      }
    },
    css: function (e, n, r, i) {
      var o, a, s, l = x.camelCase(n);
      return n = x.cssProps[l] || (x.cssProps[l] = tn(e.style, l)), s = x.cssHooks[n] || x.cssHooks[l], s && "get" in s && (a = s.get(e, !0, r)), a === t && (a = Wt(e, n, i)), "normal" === a && n in Kt && (a = Kt[n]), "" === r || r ? (o = parseFloat(a), r === !0 || x.isNumeric(o) ? o || 0 : a) : a
    }
  }), e.getComputedStyle ? (Rt = function (t) {
    return e.getComputedStyle(t, null)
  }, Wt = function (e, n, r) {
    var i, o, a, s = r || Rt(e),
      l = s ? s.getPropertyValue(n) || s[n] : t,
      u = e.style;
    return s && ("" !== l || x.contains(e.ownerDocument, e) || (l = x.style(e, n)), Yt.test(l) && Ut.test(n) && (i = u.width, o = u.minWidth, a = u.maxWidth, u.minWidth = u.maxWidth = u.width = l, l = s.width, u.width = i, u.minWidth = o, u.maxWidth = a)), l
  }) : a.documentElement.currentStyle && (Rt = function (e) {
    return e.currentStyle
  }, Wt = function (e, n, r) {
    var i, o, a, s = r || Rt(e),
      l = s ? s[n] : t,
      u = e.style;
    return null == l && u && u[n] && (l = u[n]), Yt.test(l) && !zt.test(n) && (i = u.left, o = e.runtimeStyle, a = o && o.left, a && (o.left = e.currentStyle.left), u.left = "fontSize" === n ? "1em" : l, l = u.pixelLeft + "px", u.left = i, a && (o.left = a)), "" === l ? "auto" : l
  });

  function on(e, t, n) {
    var r = Vt.exec(t);
    return r ? Math.max(0, r[1] - (n || 0)) + (r[2] || "px") : t
  }

  function an(e, t, n, r, i) {
    var o = n === (r ? "border" : "content") ? 4 : "width" === t ? 1 : 0,
      a = 0;
    for (; 4 > o; o += 2) "margin" === n && (a += x.css(e, n + Zt[o], !0, i)), r ? ("content" === n && (a -= x.css(e, "padding" + Zt[o], !0, i)), "margin" !== n && (a -= x.css(e, "border" + Zt[o] + "Width", !0, i))) : (a += x.css(e, "padding" + Zt[o], !0, i), "padding" !== n && (a += x.css(e, "border" + Zt[o] + "Width", !0, i)));
    return a
  }

  function sn(e, t, n) {
    var r = !0,
      i = "width" === t ? e.offsetWidth : e.offsetHeight,
      o = Rt(e),
      a = x.support.boxSizing && "border-box" === x.css(e, "boxSizing", !1, o);
    if (0 >= i || null == i) {
      if (i = Wt(e, t, o), (0 > i || null == i) && (i = e.style[t]), Yt.test(i)) return i;
      r = a && (x.support.boxSizingReliable || i === e.style[t]), i = parseFloat(i) || 0
    }
    return i + an(e, t, n || (a ? "border" : "content"), r, o) + "px"
  }

  function ln(e) {
    var t = a,
      n = Gt[e];
    return n || (n = un(e, t), "none" !== n && n || (Pt = (Pt || x("<iframe frameborder='0' width='0' height='0'/>").css("cssText", "display:block !important")).appendTo(t.documentElement), t = (Pt[0].contentWindow || Pt[0].contentDocument).document, t.write("<!doctype html><html><body>"), t.close(), n = un(e, t), Pt.detach()), Gt[e] = n), n
  }

  function un(e, t) {
    var n = x(t.createElement(e)).appendTo(t.body),
      r = x.css(n[0], "display");
    return n.remove(), r
  }
  x.each(["height", "width"], function (e, n) {
    x.cssHooks[n] = {
      get: function (e, r, i) {
        return r ? 0 === e.offsetWidth && Xt.test(x.css(e, "display")) ? x.swap(e, Qt, function () {
          return sn(e, n, i)
        }) : sn(e, n, i) : t
      },
      set: function (e, t, r) {
        var i = r && Rt(e);
        return on(e, t, r ? an(e, n, r, x.support.boxSizing && "border-box" === x.css(e, "boxSizing", !1, i), i) : 0)
      }
    }
  }), x.support.opacity || (x.cssHooks.opacity = {
    get: function (e, t) {
      return It.test((t && e.currentStyle ? e.currentStyle.filter : e.style.filter) || "") ? .01 * parseFloat(RegExp.$1) + "" : t ? "1" : ""
    },
    set: function (e, t) {
      var n = e.style,
        r = e.currentStyle,
        i = x.isNumeric(t) ? "alpha(opacity=" + 100 * t + ")" : "",
        o = r && r.filter || n.filter || "";
      n.zoom = 1, (t >= 1 || "" === t) && "" === x.trim(o.replace($t, "")) && n.removeAttribute && (n.removeAttribute("filter"), "" === t || r && !r.filter) || (n.filter = $t.test(o) ? o.replace($t, i) : o + " " + i)
    }
  }), x(function () {
    x.support.reliableMarginRight || (x.cssHooks.marginRight = {
      get: function (e, n) {
        return n ? x.swap(e, {
          display: "inline-block"
        }, Wt, [e, "marginRight"]) : t
      }
    }), !x.support.pixelPosition && x.fn.position && x.each(["top", "left"], function (e, n) {
      x.cssHooks[n] = {
        get: function (e, r) {
          return r ? (r = Wt(e, n), Yt.test(r) ? x(e).position()[n] + "px" : r) : t
        }
      }
    })
  }), x.expr && x.expr.filters && (x.expr.filters.hidden = function (e) {
    return 0 >= e.offsetWidth && 0 >= e.offsetHeight || !x.support.reliableHiddenOffsets && "none" === (e.style && e.style.display || x.css(e, "display"))
  }, x.expr.filters.visible = function (e) {
    return !x.expr.filters.hidden(e)
  }), x.each({
    margin: "",
    padding: "",
    border: "Width"
  }, function (e, t) {
    x.cssHooks[e + t] = {
      expand: function (n) {
        var r = 0,
          i = {},
          o = "string" == typeof n ? n.split(" ") : [n];
        for (; 4 > r; r++) i[e + Zt[r] + t] = o[r] || o[r - 2] || o[0];
        return i
      }
    }, Ut.test(e) || (x.cssHooks[e + t].set = on)
  });
  var cn = /%20/g,
    pn = /\[\]$/,
    fn = /\r?\n/g,
    dn = /^(?:submit|button|image|reset|file)$/i,
    hn = /^(?:input|select|textarea|keygen)/i;
  x.fn.extend({
    serialize: function () {
      return x.param(this.serializeArray())
    },
    serializeArray: function () {
      return this.map(function () {
        var e = x.prop(this, "elements");
        return e ? x.makeArray(e) : this
      }).filter(function () {
        var e = this.type;
        return this.name && !x(this).is(":disabled") && hn.test(this.nodeName) && !dn.test(e) && (this.checked || !Ct.test(e))
      }).map(function (e, t) {
        var n = x(this).val();
        return null == n ? null : x.isArray(n) ? x.map(n, function (e) {
          return {
            name: t.name,
            value: e.replace(fn, "\r\n")
          }
        }) : {
          name: t.name,
          value: n.replace(fn, "\r\n")
        }
      }).get()
    }
  }), x.param = function (e, n) {
    var r, i = [],
      o = function (e, t) {
        t = x.isFunction(t) ? t() : null == t ? "" : t, i[i.length] = encodeURIComponent(e) + "=" + encodeURIComponent(t)
      };
    if (n === t && (n = x.ajaxSettings && x.ajaxSettings.traditional), x.isArray(e) || e.jquery && !x.isPlainObject(e)) x.each(e, function () {
      o(this.name, this.value)
    });
    else
      for (r in e) gn(r, e[r], n, o);
    return i.join("&").replace(cn, "+")
  };

  function gn(e, t, n, r) {
    var i;
    if (x.isArray(t)) x.each(t, function (t, i) {
      n || pn.test(e) ? r(e, i) : gn(e + "[" + ("object" == typeof i ? t : "") + "]", i, n, r)
    });
    else if (n || "object" !== x.type(t)) r(e, t);
    else
      for (i in t) gn(e + "[" + i + "]", t[i], n, r)
  }
  x.each("blur focus focusin focusout load resize scroll unload click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave change select submit keydown keypress keyup error contextmenu".split(" "), function (e, t) {
    x.fn[t] = function (e, n) {
      return arguments.length > 0 ? this.on(t, null, e, n) : this.trigger(t)
    }
  }), x.fn.extend({
    hover: function (e, t) {
      return this.mouseenter(e).mouseleave(t || e)
    },
    bind: function (e, t, n) {
      return this.on(e, null, t, n)
    },
    unbind: function (e, t) {
      return this.off(e, null, t)
    },
    delegate: function (e, t, n, r) {
      return this.on(t, e, n, r)
    },
    undelegate: function (e, t, n) {
      return 1 === arguments.length ? this.off(e, "**") : this.off(t, e || "**", n)
    }
  });
  var mn, yn, vn = x.now(),
    bn = /\?/,
    xn = /#.*$/,
    wn = /([?&])_=[^&]*/,
    Tn = /^(.*?):[ \t]*([^\r\n]*)\r?$/gm,
    Cn = /^(?:about|app|app-storage|.+-extension|file|res|widget):$/,
    Nn = /^(?:GET|HEAD)$/,
    kn = /^\/\//,
    En = /^([\w.+-]+:)(?:\/\/([^\/?#:]*)(?::(\d+)|)|)/,
    Sn = x.fn.load,
    An = {},
    jn = {},
    Dn = "*/".concat("*");
  try {
    yn = o.href
  } catch (Ln) {
    yn = a.createElement("a"), yn.href = "", yn = yn.href
  }
  mn = En.exec(yn.toLowerCase()) || [];

  function Hn(e) {
    return function (t, n) {
      "string" != typeof t && (n = t, t = "*");
      var r, i = 0,
        o = t.toLowerCase().match(T) || [];
      if (x.isFunction(n))
        while (r = o[i++]) "+" === r[0] ? (r = r.slice(1) || "*", (e[r] = e[r] || []).unshift(n)) : (e[r] = e[r] || []).push(n)
    }
  }

  function qn(e, n, r, i) {
    var o = {},
      a = e === jn;

    function s(l) {
      var u;
      return o[l] = !0, x.each(e[l] || [], function (e, l) {
        var c = l(n, r, i);
        return "string" != typeof c || a || o[c] ? a ? !(u = c) : t : (n.dataTypes.unshift(c), s(c), !1)
      }), u
    }
    return s(n.dataTypes[0]) || !o["*"] && s("*")
  }

  function _n(e, n) {
    var r, i, o = x.ajaxSettings.flatOptions || {};
    for (i in n) n[i] !== t && ((o[i] ? e : r || (r = {}))[i] = n[i]);
    return r && x.extend(!0, e, r), e
  }
  x.fn.load = function (e, n, r) {
    if ("string" != typeof e && Sn) return Sn.apply(this, arguments);
    var i, o, a, s = this,
      l = e.indexOf(" ");
    return l >= 0 && (i = e.slice(l, e.length), e = e.slice(0, l)), x.isFunction(n) ? (r = n, n = t) : n && "object" == typeof n && (a = "POST"), s.length > 0 && x.ajax({
      url: e,
      type: a,
      dataType: "html",
      data: n
    }).done(function (e) {
      o = arguments, s.html(i ? x("<div>").append(x.parseHTML(e)).find(i) : e)
    }).complete(r && function (e, t) {
      s.each(r, o || [e.responseText, t, e])
    }), this
  }, x.each(["ajaxStart", "ajaxStop", "ajaxComplete", "ajaxError", "ajaxSuccess", "ajaxSend"], function (e, t) {
    x.fn[t] = function (e) {
      return this.on(t, e)
    }
  }), x.extend({
    active: 0,
    lastModified: {},
    etag: {},
    ajaxSettings: {
      url: yn,
      type: "GET",
      isLocal: Cn.test(mn[1]),
      global: !0,
      processData: !0,
      async: !0,
      contentType: "application/x-www-form-urlencoded; charset=UTF-8",
      accepts: {
        "*": Dn,
        text: "text/plain",
        html: "text/html",
        xml: "application/xml, text/xml",
        json: "application/json, text/javascript"
      },
      contents: {
        xml: /xml/,
        html: /html/,
        json: /json/
      },
      responseFields: {
        xml: "responseXML",
        text: "responseText",
        json: "responseJSON"
      },
      converters: {
        "* text": String,
        "text html": !0,
        "text json": x.parseJSON,
        "text xml": x.parseXML
      },
      flatOptions: {
        url: !0,
        context: !0
      }
    },
    ajaxSetup: function (e, t) {
      return t ? _n(_n(e, x.ajaxSettings), t) : _n(x.ajaxSettings, e)
    },
    ajaxPrefilter: Hn(An),
    ajaxTransport: Hn(jn),
    ajax: function (e, n) {
      "object" == typeof e && (n = e, e = t), n = n || {};
      var r, i, o, a, s, l, u, c, p = x.ajaxSetup({}, n),
        f = p.context || p,
        d = p.context && (f.nodeType || f.jquery) ? x(f) : x.event,
        h = x.Deferred(),
        g = x.Callbacks("once memory"),
        m = p.statusCode || {},
        y = {},
        v = {},
        b = 0,
        w = "canceled",
        C = {
          readyState: 0,
          getResponseHeader: function (e) {
            var t;
            if (2 === b) {
              if (!c) {
                c = {};
                while (t = Tn.exec(a)) c[t[1].toLowerCase()] = t[2]
              }
              t = c[e.toLowerCase()]
            }
            return null == t ? null : t
          },
          getAllResponseHeaders: function () {
            return 2 === b ? a : null
          },
          setRequestHeader: function (e, t) {
            var n = e.toLowerCase();
            return b || (e = v[n] = v[n] || e, y[e] = t), this
          },
          overrideMimeType: function (e) {
            return b || (p.mimeType = e), this
          },
          statusCode: function (e) {
            var t;
            if (e)
              if (2 > b)
                for (t in e) m[t] = [m[t], e[t]];
              else C.always(e[C.status]);
            return this
          },
          abort: function (e) {
            var t = e || w;
            return u && u.abort(t), k(0, t), this
          }
        };
      if (h.promise(C).complete = g.add, C.success = C.done, C.error = C.fail, p.url = ((e || p.url || yn) + "").replace(xn, "").replace(kn, mn[1] + "//"), p.type = n.method || n.type || p.method || p.type, p.dataTypes = x.trim(p.dataType || "*").toLowerCase().match(T) || [""], null == p.crossDomain && (r = En.exec(p.url.toLowerCase()), p.crossDomain = !(!r || r[1] === mn[1] && r[2] === mn[2] && (r[3] || ("http:" === r[1] ? "80" : "443")) === (mn[3] || ("http:" === mn[1] ? "80" : "443")))), p.data && p.processData && "string" != typeof p.data && (p.data = x.param(p.data, p.traditional)), qn(An, p, n, C), 2 === b) return C;
      l = p.global, l && 0 === x.active++ && x.event.trigger("ajaxStart"), p.type = p.type.toUpperCase(), p.hasContent = !Nn.test(p.type), o = p.url, p.hasContent || (p.data && (o = p.url += (bn.test(o) ? "%26" : "%3F") + p.data, delete p.data), p.cache === !1 && (p.url = wn.test(o) ? o.replace(wn, "$1_=" + vn++) : o + (bn.test(o) ? "%26" : "%3F") + "_=" + vn++)), p.ifModified && (x.lastModified[o] && C.setRequestHeader("If-Modified-Since", x.lastModified[o]), x.etag[o] && C.setRequestHeader("If-None-Match", x.etag[o])), (p.data && p.hasContent && p.contentType !== !1 || n.contentType) && C.setRequestHeader("Content-Type", p.contentType), C.setRequestHeader("Accept", p.dataTypes[0] && p.accepts[p.dataTypes[0]] ? p.accepts[p.dataTypes[0]] + ("*" !== p.dataTypes[0] ? ", " + Dn + "; q=0.01" : "") : p.accepts["*"]);
      for (i in p.headers) C.setRequestHeader(i, p.headers[i]);
      if (p.beforeSend && (p.beforeSend.call(f, C, p) === !1 || 2 === b)) return C.abort();
      w = "abort";
      for (i in {
          success: 1,
          error: 1,
          complete: 1
        }) C[i](p[i]);
      if (u = qn(jn, p, n, C)) {
        C.readyState = 1, l && d.trigger("ajaxSend", [C, p]), p.async && p.timeout > 0 && (s = setTimeout(function () {
          C.abort("timeout")
        }, p.timeout));
        try {
          b = 1, u.send(y, k)
        } catch (N) {
          if (!(2 > b)) throw N;
          k(-1, N)
        }
      } else k(-1, "No Transport");

      function k(e, n, r, i) {
        var c, y, v, w, T, N = n;
        2 !== b && (b = 2, s && clearTimeout(s), u = t, a = i || "", C.readyState = e > 0 ? 4 : 0, c = e >= 200 && 300 > e || 304 === e, r && (w = Mn(p, C, r)), w = On(p, w, C, c), c ? (p.ifModified && (T = C.getResponseHeader("Last-Modified"), T && (x.lastModified[o] = T), T = C.getResponseHeader("etag"), T && (x.etag[o] = T)), 204 === e || "HEAD" === p.type ? N = "nocontent" : 304 === e ? N = "notmodified" : (N = w.state, y = w.data, v = w.error, c = !v)) : (v = N, (e || !N) && (N = "error", 0 > e && (e = 0))), C.status = e, C.statusText = (n || N) + "", c ? h.resolveWith(f, [y, N, C]) : h.rejectWith(f, [C, N, v]), C.statusCode(m), m = t, l && d.trigger(c ? "ajaxSuccess" : "ajaxError", [C, p, c ? y : v]), g.fireWith(f, [C, N]), l && (d.trigger("ajaxComplete", [C, p]), --x.active || x.event.trigger("ajaxStop")))
      }
      return C
    },
    getJSON: function (e, t, n) {
      return x.get(e, t, n, "json")
    },
    getScript: function (e, n) {
      return x.get(e, t, n, "script")
    }
  }), x.each(["get", "post"], function (e, n) {
    x[n] = function (e, r, i, o) {
      return x.isFunction(r) && (o = o || i, i = r, r = t), x.ajax({
        url: e,
        type: n,
        dataType: o,
        data: r,
        success: i
      })
    }
  });

  function Mn(e, n, r) {
    var i, o, a, s, l = e.contents,
      u = e.dataTypes;
    while ("*" === u[0]) u.shift(), o === t && (o = e.mimeType || n.getResponseHeader("Content-Type"));
    if (o)
      for (s in l)
        if (l[s] && l[s].test(o)) {
          u.unshift(s);
          break
        }
    if (u[0] in r) a = u[0];
    else {
      for (s in r) {
        if (!u[0] || e.converters[s + " " + u[0]]) {
          a = s;
          break
        }
        i || (i = s)
      }
      a = a || i
    }
    return a ? (a !== u[0] && u.unshift(a), r[a]) : t
  }

  function On(e, t, n, r) {
    var i, o, a, s, l, u = {},
      c = e.dataTypes.slice();
    if (c[1])
      for (a in e.converters) u[a.toLowerCase()] = e.converters[a];
    o = c.shift();
    while (o)
      if (e.responseFields[o] && (n[e.responseFields[o]] = t), !l && r && e.dataFilter && (t = e.dataFilter(t, e.dataType)), l = o, o = c.shift())
        if ("*" === o) o = l;
        else if ("*" !== l && l !== o) {
      if (a = u[l + " " + o] || u["* " + o], !a)
        for (i in u)
          if (s = i.split(" "), s[1] === o && (a = u[l + " " + s[0]] || u["* " + s[0]])) {
            a === !0 ? a = u[i] : u[i] !== !0 && (o = s[0], c.unshift(s[1]));
            break
          }
      if (a !== !0)
        if (a && e["throws"]) t = a(t);
        else try {
          t = a(t)
        } catch (p) {
          return {
            state: "parsererror",
            error: a ? p : "No conversion from " + l + " to " + o
          }
        }
    }
    return {
      state: "success",
      data: t
    }
  }
  x.ajaxSetup({
    accepts: {
      script: "text/javascript, application/javascript, application/ecmascript, application/x-ecmascript"
    },
    contents: {
      script: /(?:java|ecma)script/
    },
    converters: {
      "text script": function (e) {
        return x.globalEval(e), e
      }
    }
  }), x.ajaxPrefilter("script", function (e) {
    e.cache === t && (e.cache = !1), e.crossDomain && (e.type = "GET", e.global = !1)
  }), x.ajaxTransport("script", function (e) {
    if (e.crossDomain) {
      var n, r = a.head || x("head")[0] || a.documentElement;
      return {
        send: function (t, i) {
          n = a.createElement("script"), n.async = !0, e.scriptCharset && (n.charset = e.scriptCharset), n.src = e.url, n.onload = n.onreadystatechange = function (e, t) {
            (t || !n.readyState || /loaded|complete/.test(n.readyState)) && (n.onload = n.onreadystatechange = null, n.parentNode && n.parentNode.removeChild(n), n = null, t || i(200, "success"))
          }, r.insertBefore(n, r.firstChild)
        },
        abort: function () {
          n && n.onload(t, !0)
        }
      }
    }
  });
  var Fn = [],
    Bn = /(=)\?(?=&|$)|\?\?/;
  x.ajaxSetup({
    jsonp: "callback",
    jsonpCallback: function () {
      var e = Fn.pop() || x.expando + "_" + vn++;
      return this[e] = !0, e
    }
  }), x.ajaxPrefilter("json jsonp", function (n, r, i) {
    var o, a, s, l = n.jsonp !== !1 && (Bn.test(n.url) ? "url" : "string" == typeof n.data && !(n.contentType || "").indexOf("application/x-www-form-urlencoded") && Bn.test(n.data) && "data");
    return l || "jsonp" === n.dataTypes[0] ? (o = n.jsonpCallback = x.isFunction(n.jsonpCallback) ? n.jsonpCallback() : n.jsonpCallback, l ? n[l] = n[l].replace(Bn, "$1" + o) : n.jsonp !== !1 && (n.url += (bn.test(n.url) ? "%26" : "%3F") + n.jsonp + "=" + o), n.converters["script json"] = function () {
      return s || x.error(o + " was not called"), s[0]
    }, n.dataTypes[0] = "json", a = e[o], e[o] = function () {
      s = arguments
    }, i.always(function () {
      e[o] = a, n[o] && (n.jsonpCallback = r.jsonpCallback, Fn.push(o)), s && x.isFunction(a) && a(s[0]), s = a = t
    }), "script") : t
  });
  var Pn, Rn, Wn = 0,
    $n = e.ActiveXObject && function () {
      var e;
      for (e in Pn) Pn[e](t, !0)
    };

  function In() {
    try {
      return new e.XMLHttpRequest
    } catch (t) {}
  }

  function zn() {
    try {
      return new e.ActiveXObject("Microsoft.XMLHTTP")
    } catch (t) {}
  }
  x.ajaxSettings.xhr = e.ActiveXObject ? function () {
    return !this.isLocal && In() || zn()
  } : In, Rn = x.ajaxSettings.xhr(), x.support.cors = !!Rn && "withCredentials" in Rn, Rn = x.support.ajax = !!Rn, Rn && x.ajaxTransport(function (n) {
    if (!n.crossDomain || x.support.cors) {
      var r;
      return {
        send: function (i, o) {
          var a, s, l = n.xhr();
          if (n.username ? l.open(n.type, n.url, n.async, n.username, n.password) : l.open(n.type, n.url, n.async), n.xhrFields)
            for (s in n.xhrFields) l[s] = n.xhrFields[s];
          n.mimeType && l.overrideMimeType && l.overrideMimeType(n.mimeType), n.crossDomain || i["X-Requested-With"] || (i["X-Requested-With"] = "XMLHttpRequest");
          try {
            for (s in i) l.setRequestHeader(s, i[s])
          } catch (u) {}
          l.send(n.hasContent && n.data || null), r = function (e, i) {
            var s, u, c, p;
            try {
              if (r && (i || 4 === l.readyState))
                if (r = t, a && (l.onreadystatechange = x.noop, $n && delete Pn[a]), i) 4 !== l.readyState && l.abort();
                else {
                  p = {}, s = l.status, u = l.getAllResponseHeaders(), "string" == typeof l.responseText && (p.text = l.responseText);
                  try {
                    c = l.statusText
                  } catch (f) {
                    c = ""
                  }
                  s || !n.isLocal || n.crossDomain ? 1223 === s && (s = 204) : s = p.text ? 200 : 404
                }
            } catch (d) {
              i || o(-1, d)
            }
            p && o(s, c, p, u)
          }, n.async ? 4 === l.readyState ? setTimeout(r) : (a = ++Wn, $n && (Pn || (Pn = {}, x(e).unload($n)), Pn[a] = r), l.onreadystatechange = r) : r()
        },
        abort: function () {
          r && r(t, !0)
        }
      }
    }
  });
  var Xn, Un, Vn = /^(?:toggle|show|hide)$/,
    Yn = RegExp("^(?:([+-])=|)(" + w + ")([a-z%]*)$", "i"),
    Jn = /queueHooks$/,
    Gn = [nr],
    Qn = {
      "*": [function (e, t) {
        var n = this.createTween(e, t),
          r = n.cur(),
          i = Yn.exec(t),
          o = i && i[3] || (x.cssNumber[e] ? "" : "px"),
          a = (x.cssNumber[e] || "px" !== o && +r) && Yn.exec(x.css(n.elem, e)),
          s = 1,
          l = 20;
        if (a && a[3] !== o) {
          o = o || a[3], i = i || [], a = +r || 1;
          do s = s || ".5", a /= s, x.style(n.elem, e, a + o); while (s !== (s = n.cur() / r) && 1 !== s && --l)
        }
        return i && (a = n.start = +a || +r || 0, n.unit = o, n.end = i[1] ? a + (i[1] + 1) * i[2] : +i[2]), n
      }]
    };

  function Kn() {
    return setTimeout(function () {
      Xn = t
    }), Xn = x.now()
  }

  function Zn(e, t, n) {
    var r, i = (Qn[t] || []).concat(Qn["*"]),
      o = 0,
      a = i.length;
    for (; a > o; o++)
      if (r = i[o].call(n, t, e)) return r
  }

  function er(e, t, n) {
    var r, i, o = 0,
      a = Gn.length,
      s = x.Deferred().always(function () {
        delete l.elem
      }),
      l = function () {
        if (i) return !1;
        var t = Xn || Kn(),
          n = Math.max(0, u.startTime + u.duration - t),
          r = n / u.duration || 0,
          o = 1 - r,
          a = 0,
          l = u.tweens.length;
        for (; l > a; a++) u.tweens[a].run(o);
        return s.notifyWith(e, [u, o, n]), 1 > o && l ? n : (s.resolveWith(e, [u]), !1)
      },
      u = s.promise({
        elem: e,
        props: x.extend({}, t),
        opts: x.extend(!0, {
          specialEasing: {}
        }, n),
        originalProperties: t,
        originalOptions: n,
        startTime: Xn || Kn(),
        duration: n.duration,
        tweens: [],
        createTween: function (t, n) {
          var r = x.Tween(e, u.opts, t, n, u.opts.specialEasing[t] || u.opts.easing);
          return u.tweens.push(r), r
        },
        stop: function (t) {
          var n = 0,
            r = t ? u.tweens.length : 0;
          if (i) return this;
          for (i = !0; r > n; n++) u.tweens[n].run(1);
          return t ? s.resolveWith(e, [u, t]) : s.rejectWith(e, [u, t]), this
        }
      }),
      c = u.props;
    for (tr(c, u.opts.specialEasing); a > o; o++)
      if (r = Gn[o].call(u, e, c, u.opts)) return r;
    return x.map(c, Zn, u), x.isFunction(u.opts.start) && u.opts.start.call(e, u), x.fx.timer(x.extend(l, {
      elem: e,
      anim: u,
      queue: u.opts.queue
    })), u.progress(u.opts.progress).done(u.opts.done, u.opts.complete).fail(u.opts.fail).always(u.opts.always)
  }

  function tr(e, t) {
    var n, r, i, o, a;
    for (n in e)
      if (r = x.camelCase(n), i = t[r], o = e[n], x.isArray(o) && (i = o[1], o = e[n] = o[0]), n !== r && (e[r] = o, delete e[n]), a = x.cssHooks[r], a && "expand" in a) {
        o = a.expand(o), delete e[r];
        for (n in o) n in e || (e[n] = o[n], t[n] = i)
      } else t[r] = i
  }
  x.Animation = x.extend(er, {
    tweener: function (e, t) {
      x.isFunction(e) ? (t = e, e = ["*"]) : e = e.split(" ");
      var n, r = 0,
        i = e.length;
      for (; i > r; r++) n = e[r], Qn[n] = Qn[n] || [], Qn[n].unshift(t)
    },
    prefilter: function (e, t) {
      t ? Gn.unshift(e) : Gn.push(e)
    }
  });

  function nr(e, t, n) {
    var r, i, o, a, s, l, u = this,
      c = {},
      p = e.style,
      f = e.nodeType && nn(e),
      d = x._data(e, "fxshow");
    n.queue || (s = x._queueHooks(e, "fx"), null == s.unqueued && (s.unqueued = 0, l = s.empty.fire, s.empty.fire = function () {
      s.unqueued || l()
    }), s.unqueued++, u.always(function () {
      u.always(function () {
        s.unqueued--, x.queue(e, "fx").length || s.empty.fire()
      })
    })), 1 === e.nodeType && ("height" in t || "width" in t) && (n.overflow = [p.overflow, p.overflowX, p.overflowY], "inline" === x.css(e, "display") && "none" === x.css(e, "float") && (x.support.inlineBlockNeedsLayout && "inline" !== ln(e.nodeName) ? p.zoom = 1 : p.display = "inline-block")), n.overflow && (p.overflow = "hidden", x.support.shrinkWrapBlocks || u.always(function () {
      p.overflow = n.overflow[0], p.overflowX = n.overflow[1], p.overflowY = n.overflow[2]
    }));
    for (r in t)
      if (i = t[r], Vn.exec(i)) {
        if (delete t[r], o = o || "toggle" === i, i === (f ? "hide" : "show")) continue;
        c[r] = d && d[r] || x.style(e, r)
      }
    if (!x.isEmptyObject(c)) {
      d ? "hidden" in d && (f = d.hidden) : d = x._data(e, "fxshow", {}), o && (d.hidden = !f), f ? x(e).show() : u.done(function () {
        x(e).hide()
      }), u.done(function () {
        var t;
        x._removeData(e, "fxshow");
        for (t in c) x.style(e, t, c[t])
      });
      for (r in c) a = Zn(f ? d[r] : 0, r, u), r in d || (d[r] = a.start, f && (a.end = a.start, a.start = "width" === r || "height" === r ? 1 : 0))
    }
  }

  function rr(e, t, n, r, i) {
    return new rr.prototype.init(e, t, n, r, i)
  }
  x.Tween = rr, rr.prototype = {
    constructor: rr,
    init: function (e, t, n, r, i, o) {
      this.elem = e, this.prop = n, this.easing = i || "swing", this.options = t, this.start = this.now = this.cur(), this.end = r, this.unit = o || (x.cssNumber[n] ? "" : "px")
    },
    cur: function () {
      var e = rr.propHooks[this.prop];
      return e && e.get ? e.get(this) : rr.propHooks._default.get(this)
    },
    run: function (e) {
      var t, n = rr.propHooks[this.prop];
      return this.pos = t = this.options.duration ? x.easing[this.easing](e, this.options.duration * e, 0, 1, this.options.duration) : e, this.now = (this.end - this.start) * t + this.start, this.options.step && this.options.step.call(this.elem, this.now, this), n && n.set ? n.set(this) : rr.propHooks._default.set(this), this
    }
  }, rr.prototype.init.prototype = rr.prototype, rr.propHooks = {
    _default: {
      get: function (e) {
        var t;
        return null == e.elem[e.prop] || e.elem.style && null != e.elem.style[e.prop] ? (t = x.css(e.elem, e.prop, ""), t && "auto" !== t ? t : 0) : e.elem[e.prop]
      },
      set: function (e) {
        x.fx.step[e.prop] ? x.fx.step[e.prop](e) : e.elem.style && (null != e.elem.style[x.cssProps[e.prop]] || x.cssHooks[e.prop]) ? x.style(e.elem, e.prop, e.now + e.unit) : e.elem[e.prop] = e.now
      }
    }
  }, rr.propHooks.scrollTop = rr.propHooks.scrollLeft = {
    set: function (e) {
      e.elem.nodeType && e.elem.parentNode && (e.elem[e.prop] = e.now)
    }
  }, x.each(["toggle", "show", "hide"], function (e, t) {
    var n = x.fn[t];
    x.fn[t] = function (e, r, i) {
      return null == e || "boolean" == typeof e ? n.apply(this, arguments) : this.animate(ir(t, !0), e, r, i)
    }
  }), x.fn.extend({
    fadeTo: function (e, t, n, r) {
      return this.filter(nn).css("opacity", 0).show().end().animate({
        opacity: t
      }, e, n, r)
    },
    animate: function (e, t, n, r) {
      var i = x.isEmptyObject(e),
        o = x.speed(t, n, r),
        a = function () {
          var t = er(this, x.extend({}, e), o);
          (i || x._data(this, "finish")) && t.stop(!0)
        };
      return a.finish = a, i || o.queue === !1 ? this.each(a) : this.queue(o.queue, a)
    },
    stop: function (e, n, r) {
      var i = function (e) {
        var t = e.stop;
        delete e.stop, t(r)
      };
      return "string" != typeof e && (r = n, n = e, e = t), n && e !== !1 && this.queue(e || "fx", []), this.each(function () {
        var t = !0,
          n = null != e && e + "queueHooks",
          o = x.timers,
          a = x._data(this);
        if (n) a[n] && a[n].stop && i(a[n]);
        else
          for (n in a) a[n] && a[n].stop && Jn.test(n) && i(a[n]);
        for (n = o.length; n--;) o[n].elem !== this || null != e && o[n].queue !== e || (o[n].anim.stop(r), t = !1, o.splice(n, 1));
        (t || !r) && x.dequeue(this, e)
      })
    },
    finish: function (e) {
      return e !== !1 && (e = e || "fx"), this.each(function () {
        var t, n = x._data(this),
          r = n[e + "queue"],
          i = n[e + "queueHooks"],
          o = x.timers,
          a = r ? r.length : 0;
        for (n.finish = !0, x.queue(this, e, []), i && i.stop && i.stop.call(this, !0), t = o.length; t--;) o[t].elem === this && o[t].queue === e && (o[t].anim.stop(!0), o.splice(t, 1));
        for (t = 0; a > t; t++) r[t] && r[t].finish && r[t].finish.call(this);
        delete n.finish
      })
    }
  });

  function ir(e, t) {
    var n, r = {
        height: e
      },
      i = 0;
    for (t = t ? 1 : 0; 4 > i; i += 2 - t) n = Zt[i], r["margin" + n] = r["padding" + n] = e;
    return t && (r.opacity = r.width = e), r
  }
  x.each({
    slideDown: ir("show"),
    slideUp: ir("hide"),
    slideToggle: ir("toggle"),
    fadeIn: {
      opacity: "show"
    },
    fadeOut: {
      opacity: "hide"
    },
    fadeToggle: {
      opacity: "toggle"
    }
  }, function (e, t) {
    x.fn[e] = function (e, n, r) {
      return this.animate(t, e, n, r)
    }
  }), x.speed = function (e, t, n) {
    var r = e && "object" == typeof e ? x.extend({}, e) : {
      complete: n || !n && t || x.isFunction(e) && e,
      duration: e,
      easing: n && t || t && !x.isFunction(t) && t
    };
    return r.duration = x.fx.off ? 0 : "number" == typeof r.duration ? r.duration : r.duration in x.fx.speeds ? x.fx.speeds[r.duration] : x.fx.speeds._default, (null == r.queue || r.queue === !0) && (r.queue = "fx"), r.old = r.complete, r.complete = function () {
      x.isFunction(r.old) && r.old.call(this), r.queue && x.dequeue(this, r.queue)
    }, r
  }, x.easing = {
    linear: function (e) {
      return e
    },
    swing: function (e) {
      return .5 - Math.cos(e * Math.PI) / 2
    }
  }, x.timers = [], x.fx = rr.prototype.init, x.fx.tick = function () {
    var e, n = x.timers,
      r = 0;
    for (Xn = x.now(); n.length > r; r++) e = n[r], e() || n[r] !== e || n.splice(r--, 1);
    n.length || x.fx.stop(), Xn = t
  }, x.fx.timer = function (e) {
    e() && x.timers.push(e) && x.fx.start()
  }, x.fx.interval = 13, x.fx.start = function () {
    Un || (Un = setInterval(x.fx.tick, x.fx.interval))
  }, x.fx.stop = function () {
    clearInterval(Un), Un = null
  }, x.fx.speeds = {
    slow: 600,
    fast: 200,
    _default: 400
  }, x.fx.step = {}, x.expr && x.expr.filters && (x.expr.filters.animated = function (e) {
    return x.grep(x.timers, function (t) {
      return e === t.elem
    }).length
  }), x.fn.offset = function (e) {
    if (arguments.length) return e === t ? this : this.each(function (t) {
      x.offset.setOffset(this, e, t)
    });
    var n, r, o = {
        top: 0,
        left: 0
      },
      a = this[0],
      s = a && a.ownerDocument;
    if (s) return n = s.documentElement, x.contains(n, a) ? (typeof a.getBoundingClientRect !== i && (o = a.getBoundingClientRect()), r = or(s), {
      top: o.top + (r.pageYOffset || n.scrollTop) - (n.clientTop || 0),
      left: o.left + (r.pageXOffset || n.scrollLeft) - (n.clientLeft || 0)
    }) : o
  }, x.offset = {
    setOffset: function (e, t, n) {
      var r = x.css(e, "position");
      "static" === r && (e.style.position = "relative");
      var i = x(e),
        o = i.offset(),
        a = x.css(e, "top"),
        s = x.css(e, "left"),
        l = ("absolute" === r || "fixed" === r) && x.inArray("auto", [a, s]) > -1,
        u = {},
        c = {},
        p, f;
      l ? (c = i.position(), p = c.top, f = c.left) : (p = parseFloat(a) || 0, f = parseFloat(s) || 0), x.isFunction(t) && (t = t.call(e, n, o)), null != t.top && (u.top = t.top - o.top + p), null != t.left && (u.left = t.left - o.left + f), "using" in t ? t.using.call(e, u) : i.css(u)
    }
  }, x.fn.extend({
    position: function () {
      if (this[0]) {
        var e, t, n = {
            top: 0,
            left: 0
          },
          r = this[0];
        return "fixed" === x.css(r, "position") ? t = r.getBoundingClientRect() : (e = this.offsetParent(), t = this.offset(), x.nodeName(e[0], "html") || (n = e.offset()), n.top += x.css(e[0], "borderTopWidth", !0), n.left += x.css(e[0], "borderLeftWidth", !0)), {
          top: t.top - n.top - x.css(r, "marginTop", !0),
          left: t.left - n.left - x.css(r, "marginLeft", !0)
        }
      }
    },
    offsetParent: function () {
      return this.map(function () {
        var e = this.offsetParent || s;
        while (e && !x.nodeName(e, "html") && "static" === x.css(e, "position")) e = e.offsetParent;
        return e || s
      })
    }
  }), x.each({
    scrollLeft: "pageXOffset",
    scrollTop: "pageYOffset"
  }, function (e, n) {
    var r = /Y/.test(n);
    x.fn[e] = function (i) {
      return x.access(this, function (e, i, o) {
        var a = or(e);
        return o === t ? a ? n in a ? a[n] : a.document.documentElement[i] : e[i] : (a ? a.scrollTo(r ? x(a).scrollLeft() : o, r ? o : x(a).scrollTop()) : e[i] = o, t)
      }, e, i, arguments.length, null)
    }
  });

  function or(e) {
    return x.isWindow(e) ? e : 9 === e.nodeType ? e.defaultView || e.parentWindow : !1
  }
  x.each({
    Height: "height",
    Width: "width"
  }, function (e, n) {
    x.each({
      padding: "inner" + e,
      content: n,
      "": "outer" + e
    }, function (r, i) {
      x.fn[i] = function (i, o) {
        var a = arguments.length && (r || "boolean" != typeof i),
          s = r || (i === !0 || o === !0 ? "margin" : "border");
        return x.access(this, function (n, r, i) {
          var o;
          return x.isWindow(n) ? n.document.documentElement["client" + e] : 9 === n.nodeType ? (o = n.documentElement, Math.max(n.body["scroll" + e], o["scroll" + e], n.body["offset" + e], o["offset" + e], o["client" + e])) : i === t ? x.css(n, r, s) : x.style(n, r, i, s)
        }, n, a ? i : t, a, null)
      }
    })
  }), x.fn.size = function () {
    return this.length
  }, x.fn.andSelf = x.fn.addBack, "object" == typeof module && module && "object" == typeof module.exports ? module.exports = x : (e.jQuery = e.$ = x, "function" == typeof define && define.amd && define("jquery", [], function () {
    return x
  }))
})(window);
! function (a, b, c, d) {
  var e = a(b);
  a.fn.lazyload = function (f) {
    function g() {
      var b = 0;
      i.each(function () {
        var c = a(this);
        if (!j.skip_invisible || c.is(":visible"))
          if (a.abovethetop(this, j) || a.leftofbegin(this, j));
          else if (a.belowthefold(this, j) || a.rightoffold(this, j)) {
          if (++b > j.failure_limit) return !1
        } else c.trigger("appear"), b = 0
      })
    }
    var h, i = this,
      j = {
        threshold: 0,
        failure_limit: 0,
        event: "scroll",
        effect: "show",
        container: b,
        data_attribute: "original",
        skip_invisible: !1,
        appear: null,
        load: null,
        placeholder: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAANSURBVBhXYzh8+PB/AAffA0nNPuCLAAAAAElFTkSuQmCC"
      };
    return f && (d !== f.failurelimit && (f.failure_limit = f.failurelimit, delete f.failurelimit), d !== f.effectspeed && (f.effect_speed = f.effectspeed, delete f.effectspeed), a.extend(j, f)), h = j.container === d || j.container === b ? e : a(j.container), 0 === j.event.indexOf("scroll") && h.bind(j.event, function () {
      return g()
    }), this.each(function () {
      var b = this,
        c = a(b);
      b.loaded = !1, (c.attr("src") === d || c.attr("src") === !1) && c.is("img") && c.attr("src", j.placeholder), c.one("appear", function () {
        if (!this.loaded) {
          if (j.appear) {
            var d = i.length;
            j.appear.call(b, d, j)
          }
          a("<img />").bind("load", function () {
            var d = c.attr("data-" + j.data_attribute);
            c.hide(), c.is("img") ? c.attr("src", d) : c.css("background-image", "url('" + d + "')"), c[j.effect](j.effect_speed), b.loaded = !0;
            var e = a.grep(i, function (a) {
              return !a.loaded
            });
            if (i = a(e), j.load) {
              var f = i.length;
              j.load.call(b, f, j)
            }
          }).attr("src", c.attr("data-" + j.data_attribute))
        }
      }), 0 !== j.event.indexOf("scroll") && c.bind(j.event, function () {
        b.loaded || c.trigger("appear")
      })
    }), e.bind("resize", function () {
      g()
    }), /(?:iphone|ipod|ipad).*os 5/gi.test(navigator.appVersion) && e.bind("pageshow", function (b) {
      b.originalEvent && b.originalEvent.persisted && i.each(function () {
        a(this).trigger("appear")
      })
    }), a(c).ready(function () {
      g()
    }), this
  }, a.belowthefold = function (c, f) {
    var g;
    return g = f.container === d || f.container === b ? (b.innerHeight ? b.innerHeight : e.height()) + e.scrollTop() : a(f.container).offset().top + a(f.container).height(), g <= a(c).offset().top - f.threshold
  }, a.rightoffold = function (c, f) {
    var g;
    return g = f.container === d || f.container === b ? e.width() + e.scrollLeft() : a(f.container).offset().left + a(f.container).width(), g <= a(c).offset().left - f.threshold
  }, a.abovethetop = function (c, f) {
    var g;
    return g = f.container === d || f.container === b ? e.scrollTop() : a(f.container).offset().top, g >= a(c).offset().top + f.threshold + a(c).height()
  }, a.leftofbegin = function (c, f) {
    var g;
    return g = f.container === d || f.container === b ? e.scrollLeft() : a(f.container).offset().left, g >= a(c).offset().left + f.threshold + a(c).width()
  }, a.inviewport = function (b, c) {
    return !(a.rightoffold(b, c) || a.leftofbegin(b, c) || a.belowthefold(b, c) || a.abovethetop(b, c))
  }, a.extend(a.expr[":"], {
    "below-the-fold": function (b) {
      return a.belowthefold(b, {
        threshold: 0
      })
    },
    "above-the-top": function (b) {
      return !a.belowthefold(b, {
        threshold: 0
      })
    },
    "right-of-screen": function (b) {
      return a.rightoffold(b, {
        threshold: 0
      })
    },
    "left-of-screen": function (b) {
      return !a.rightoffold(b, {
        threshold: 0
      })
    },
    "in-viewport": function (b) {
      return a.inviewport(b, {
        threshold: 0
      })
    },
    "above-the-fold": function (b) {
      return !a.belowthefold(b, {
        threshold: 0
      })
    },
    "right-of-fold": function (b) {
      return a.rightoffold(b, {
        threshold: 0
      })
    },
    "left-of-fold": function (b) {
      return !a.rightoffold(b, {
        threshold: 0
      })
    }
  })
}(jQuery, window, document);
! function (t, s) {
  "function" == typeof define && define.amd ? define([], s) : "object" == typeof exports ? module.exports = s() : t.ToProgress = s()
}(this, function () {
  function t() {
    var t, s = document.createElement("fakeelement"),
      i = {
        transition: "transitionend",
        OTransition: "oTransitionEnd",
        MozTransition: "transitionend",
        WebkitTransition: "webkitTransitionEnd"
      };
    for (t in i)
      if (void 0 !== s.style[t]) return i[t]
  }

  function s(t, s) {
    if (this.progress = 0, this.options = {
        id: "top-progress-bar",
        color: "#F44336",
        height: "2px",
        duration: .2
      }, t && "object" == typeof t)
      for (var i in t) this.options[i] = t[i];
    if (this.options.opacityDuration = 3 * this.options.duration, this.progressBar = document.createElement("div"), this.progressBar.id = this.options.id, this.progressBar.setCSS = function (t) {
        for (var s in t) this.style[s] = t[s]
      }, this.progressBar.setCSS({
        position: s ? "relative" : "fixed",
        top: "0",
        left: "0",
        right: "0",
        "background-color": this.options.color,
        height: this.options.height,
        width: "0%",
        transition: "width " + this.options.duration + "s, opacity " + this.options.opacityDuration + "s",
        "-moz-transition": "width " + this.options.duration + "s, opacity " + this.options.opacityDuration + "s",
        "-webkit-transition": "width " + this.options.duration + "s, opacity " + this.options.opacityDuration + "s"
      }), s) {
      var o = document.querySelector(s);
      o && (o.hasChildNodes() ? o.insertBefore(this.progressBar, o.firstChild) : o.appendChild(this.progressBar))
    } else document.body.appendChild(this.progressBar)
  }
  var i = t();
  return s.prototype.transit = function () {
    this.progressBar.style.width = this.progress + "%"
  }, s.prototype.getProgress = function () {
    return this.progress
  }, s.prototype.setProgress = function (t, s) {
    this.show(), this.progress = t > 100 ? 100 : 0 > t ? 0 : t, this.transit(), s && s()
  }, s.prototype.increase = function (t, s) {
    this.show(), this.setProgress(this.progress + t, s)
  }, s.prototype.decrease = function (t, s) {
    this.show(), this.setProgress(this.progress - t, s)
  }, s.prototype.finish = function (t) {
    var s = this;
    this.setProgress(100, t), this.hide(), i && this.progressBar.addEventListener(i, function (t) {
      s.reset(), s.progressBar.removeEventListener(t.type, arguments.callee)
    })
  }, s.prototype.reset = function (t) {
    this.progress = 0, this.transit(), t && t()
  }, s.prototype.hide = function () {
    this.progressBar.style.opacity = "0"
  }, s.prototype.show = function () {
    this.progressBar.style.opacity = "1"
  }, s
});
(function (window, undefined) {
  var _5ch = {
    regexp: {
      sssp: {
        BE: /((sssp):\/\/img\.(2ch|5ch)\.net\/(ico|premium|emoji)\/[_\-a-z0-9]*\.gif)/,
        oekaki: /((sssp):\/\/([a-z]\.8ch\.net\/[a-z0-9]*\.png))/g
      },
      urls: {
        video: {
          youtube: /[www\.]*?youtu\.?be[\.?com]*\/[watch\?v=]*([\w-_]{10,12})/,
          niconico: /\/\/w?w?w?\.?nicovideo\.jp\/watch\/([\w\-]+)/
        },
        all: /(h?t?f?tps?:\/\/[-~\/A-Za-z0-9_.${}#%,:;@?%&|=\(\)\!+]+)/g,
        image: /((h?t?tps?:\/\/.+)(jpg|jpeg|bmp|png|gif))/i,
        twitpic: /h?t?tp:\/\/twitpic[.]com\/(\w+)/,
        imgur: /h?t?tps?:\/\/\w*?[.]?imgur[.]com\/(\w+)\.(jpg|jpeg|png)/,
        instagram: /h?t?tp:\/\/instagr[.]am\/p\/([\w\-]+)\//
      },
      response_anchor: /(&gt;&gt;)([0-9]+)/g,
      response_bomb: /(?:[>＞]{1,2}[0-9０-９].*?){6,}/,
      AA: /(　　＼|　　／|＼　　|／　　|￣￣|｜　　|　　｜|\|　　|　　\|)/
    },
    eventTrack: function (category, action, label, value) {
      if (!window.ga) return false;
      ga('send', 'event', category, action, label, value);
    },
    env: {
      browsing_mode: 'normal',
      controller: '',
      ad_subback_interval: 21,
      ad_interval: 18,
      pc_ads_count: 0,
      max_pc_ads_count: 5,
      version: 7,
      site_type: '5ch',
      board_type: '',
      pc: !/Android|webOS|iPhone|iPad|iPod|BlackBerry|BB|PlayBook|IEMobile|Windows Phone|Kindle|Silk|Opera Mini/i.test(navigator.userAgent),
      location_origin: '/easter?est_view=itest.5ch.net',
      ad_location_origin: location.protocol + '//' + (!!location.hostname.match(/2ch|5ch/) ? 'itest' : 'bbspink') + '.thench.net',
      api: true,
      high_speed_view: null,
      debugmode: function () {
        return !!location.search.match(/^\?d$/);
      },
      use_localstrage: function () {
        return _5ch.env.browsing_mode == 'normal';
      },
      set_browsing_mode: function () {
        try {
          sessionStorage.setItem('mode_check', '1');
        } catch (e) {
          _5ch.env.browsing_mode = 'private';
        }
      },
      set_site_type: function () {
        _5ch.env.site_type = (!!location.hostname.match(/2ch|5ch/)) ? '5ch' : 'bbspink';
      },
      set_board_type: function () {
        var domain = $('#board_domain').val();
        if (domain) {
          _5ch.env.board_type = !!domain.match(/bbspink/) ? 'bbspink' : '5ch';
        } else {
          _5ch.env.board_type = '5ch';
        }
      },
      checkCookies: function () {
        if (document.cookie.length > 3900) {
          _5ch.env.deleteCookiesUntilSafeLine('_');
        }
      },
      deleteCookiesUntilSafeLine: function (target) {
        var cookies = document.cookie.split(";");
        for (var i = 0; i < cookies.length; i++) {
          var cookie = cookies[i];
          var eq = cookie.indexOf("=");
          var name = eq > -1 ? cookie.substr(0, eq) : cookie;
          console.log(name);
          if (target) {
            if (name.indexOf(target) > -1) document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
          } {
            document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
          }
          if (3000 > document.cookie.length) break;
        }
      }
    },
    template: {
      ejs: {
        compile: function (template_selector) {
          return ejs.compile($(template_selector).html());
        }
      },
      subback: {
        ejs: {
          checkAdInterval: function (index) {
            interval++;
            var interval = (_5ch.env.site_type === 'bbspink') ? _5ch.env.ad_subback_interval : _5ch.env.ad_interval;
            return (((index + 1) % interval) == 0);
          },
          checkAdData: function () {
            return _5ch.Ad.check_data();
          },
          checkDatTime: function (board_and_datnumber) {
            var dat = board_and_datnumber.split('/').pop();
            return (_5ch.time.unix() > dat);
          },
          datToTime: function (board_and_datnumber) {
            var dat = board_and_datnumber.split('/').pop();
            return _5ch.time.unix_to_5chformat(dat);
          },
          hasThumbnail: function (board_and_datnumber) {
            var dat = board_and_datnumber.split('/').pop();
            return !!_5ch.Subback.getThumbnailURLByDatNumber(dat);
          },
          boardScreenName: function (board_and_datnumber) {
            return board_and_datnumber.split('/').shift();
          },
          subDomain: function (subdomain) {
            if (subdomain) return subdomain;
            return _5ch.Subback.getSubDomain();
          },
          checkl50: function () {
            return (_5ch.db.get('use_l50') == 'true');
          },
          ikioi: function (thread) {
            var dat = thread[3].split('/').pop();
            var res = thread[1];
            return Math.ceil(res / ((parseInt((new Date) / 1000) - dat) / 86400));
          },
          datNumber: function (board_and_datnumber) {
            return board_and_datnumber.split('/').pop();
          },
          kidoku: function (board_and_datnumber) {
            return $.cookie(board_and_datnumber.replace('/', '_'));
          },
          checkKidoku: function (board_and_datnumber) {
            return !!$.cookie(board_and_datnumber.replace('/', '_'));
          },
          checkMidoku: function (thread) {
            var kidoku = $.cookie(thread[3].replace('/', '_'));
            if (kidoku !== null) {
              if (thread[1] > kidoku) return true;
            }
            return false;
          },
          midoku: function (thread) {
            return thread[1] - $.cookie(thread[3].replace('/', '_'));
          },
          title: function (title) {
            return title.replace(/\[[0-9]+\]$/, '');
          },
          threadThumbnail: function (board_and_datnumber) {
            var dat = board_and_datnumber.split('/').pop();
            return _5ch.Subback.getThumbnailURLByDatNumber(dat);
          },
          domain: function (site_type) {
            return site_type === 'bbspink' ? 'itest.bbspink.com' : 'itest.5ch.net';
          },
          randomAdHtml: function (index) {
            return _5ch.Ad.Subback.to_html(_5ch.Ad.random(), index);
          },
          checkTopAdInterval: function (index) {
            return (index == 0);
          },
          checkNativeAdData: function () {
            var tags = _5ch.Ad.tags.subback;
            return !(tags.native_ad_1 === '' && tags.native_ad_2 === '' && tags.native_ad_3 === '' && tags.native_ad_4 === '');
          },
          checkNativeAdInterval: function (index) {
            if (_5ch.env.site_type != '5ch') return false;
            switch (index) {
            case 3:
              return true;
            case 11:
              return true;
            case 20:
              return true;
            case 28:
              return true;
            default:
              return false;
            }
          }
        },
      },
      thread: {
        ejs: {
          showCommentCount: function (now, total) {
            total = total - 0;
            if (total === 1) return "";
            return '(' + now + '/' + total + ')';
          },
          checkIDOver1: function (total) {
            total = total - 0;
            return (total > 1);
          },
          checkAdInterval: function (index) {
            index++;
            return ((index % _5ch.env.ad_interval) == 0);
          },
          checkAdData: function () {
            return _5ch.Ad.check_data();
          },
          randomAdHtml: function (index) {
            return _5ch.Ad.Thread.to_html(_5ch.Ad.random(), index);
          },
          isResNumber1: function (id) {
            return (id == 1);
          },
          zeroPaddingedResponseNumber: function (number) {
            return _5ch.number.zero_padding(4, number);
          },
          CleaningName: function (name) {
            return name.replace(/<\/?b>/g, '');
          },
          formatedCreatedAt: function (created_at) {
            return ($.blank(created_at)) ? "" : _5ch.time.unix_to_string(created_at);
          },
          responseBodyClasses: function (comment) {
            var isaa = !!comment.match(_5ch.regexp.AA);
            return isaa ? 'threadview_response_body threadview_response_body-AA' : 'threadview_response_body';
          },
          checkHasResponses: function (comment) {
            return (comment[7] > 0);
          },
          checkHasResponsesOver5: function (comment) {
            return (comment[7] >= 5);
          },
          decorateComment: function (body) {
            var be_id = body[5];
            var comment = body[6];
            comment = _5ch.Thread.convert.url_to_href(comment);
            comment = _5ch.Thread.convert.oekaki(comment);
            comment = _5ch.Thread.convert.response_anchor_to_href(comment);
            comment = _5ch.Thread.convert.BE(comment, be_id);
            return comment;
          },
          thumbnails: function (comment) {
            var thumbnail_html = _5ch.Thread.convert.create_image_thumbnail_html(comment.match(_5ch.regexp.urls.all));
            return (thumbnail_html) ? '<br />' + thumbnail_html : "";
          },
          videos: function (comment) {
            var thumbnail_html = _5ch.Thread.convert.create_video_embed_html(comment.match(_5ch.regexp.urls.all));
            return (thumbnail_html) ? '<br />' + thumbnail_html : "";
          }
        },
      },
    },
    request: {
      server_name: location.protocol + '//' + location.host + '/',
      subdomain: "",
      dat_number: "",
      board: "",
      shortcut: "",
      transfer: function (path) {
        window.location = path;
      }
    },
    error_messages: {
      subback: {
        disconnect: '<li><p class="disconnect">見つかりませんでした</p></li>',
        reload_fail: '<li><p class="disconnect">リロードに失敗しました</p></li>'
      },
      thread: {
        disconnect: '<div id="error_message">頑張って接続を試みましたが、お返事を頂けません(´;ω;`)<br />このスレッドは過去ログ、規制、エラー、負荷過多など何らかの理由により正しく所得することが出来ませんでした。もしアクティブスレッドであるならば、暫く待ってから<a href="./">再接続</a>をお試し下さい。<br /><br />戻るボタンで元の画面に戻ります。</div>',
      },
      search: {
        disconnect: '<li><p class="disconnect">ダウンロードに失敗しました。PC画面に転送します。</p></li>'
      }
    },
    DOM: {
      toResponseNumber: function (dom) {
        return dom.id.match(/[0-9]+/)[0] - 0;
      }
    },
    Loading: {
      show: function () {
        var progressBar = new ToProgress({
          id: 'top-progress-bar',
          color: '#FF9025',
          height: '2px',
          duration: 0.2
        });
        fake_increase = 5;
        progressBar.setProgress(fake_increase);
        i = setInterval(function () {
          if (fake_increase >= 90) {
            clearInterval(i);
          }
          fake_increase += 2.5;
          if (Math.round(Math.random() * (3 - 1) + 1) === 3) progressBar.setProgress(fake_increase);
        }, 200);
        $('.superloading').removeClass('hidden');
      },
      hide: function () {
        $('#top-progress-bar').remove();
        $('.superloading').addClass('hidden');
      }
    },
    text: {
      genThumbnailUrl: function (url) {
        var str = "thench" + url;
        var sha1 = (new jsSHA(str, 'TEXT')).getHash('SHA-1', 'HEX');
        var base = parseInt(sha1.substr(-2), 16).toString(3);
        return "http://thumb" + base.substr(-1) + ".thench.net/app/show_thumbnail?url=" + encodeURIComponent(url);
      },
      middle_ellipsis: function (text, max_length) {
        if (!text) return '';
        if (!max_length) max_length = 40;
        if (text.length > max_length) text = text.substring(0, Math.floor(max_length / 2)) + '...' + text.slice(-Math.abs(max_length / 2));
        return text;
      },
      trim: function (text) {
        return text.replace(/(^\s+)|(\s+$)/g, '');
      },
      rand: function () {
        var cons = 'ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz0123456789';
        var p = '';
        for (var i = 10; i > 0; i--) {
          p += cons.charAt(Math.floor(Math.random() * cons.length));
        }
        return p;
      }
    },
    number: {
      zero_padding: function (digit, number) {
        number += '';
        if (number.length > digit) return number;
        return ("0000000000" + number).slice(-Math.abs(digit));
      },
      random: function (max) {
        return Math.floor(Math.random() * max + 1);
      }
    },
    time: {
      unix: function () {
        return parseInt((new Date) / 1000);
      },
      unix_to_string: function (unixtime) {
        var tdd = _5ch.number.zero_padding;
        var date = new Date(unixtime * 1000);
        return date.getFullYear() + '/' + tdd(2, date.getMonth() + 1) + '/' + tdd(2, date.getDate()) + ' ' + tdd(2, date.getHours()) + ':' + tdd(2, date.getMinutes()) + ':' + tdd(2, date.getSeconds());
      },
      unix_to_5chformat: function (unixtime) {
        var date = new Date(unixtime * 1000);
        return date.getFullYear() + "年" + (date.getMonth() + 1) + "月" + date.getDate() + "日 " + date.getHours() + "時" + date.getMinutes() + "分";
      }
    },
    JSON: {
      check: function (obj) {
        var r = Object.prototype.toString.call(obj);
        return (r == "[object Object]" || r == "[object Array]");
      },
    },
    navButton: {
      hide: function () {
        setTimeout(function () {
          var $floatNav = $('.float-nav');
          $floatNav.find('.fa.fa-times').removeClass('fa-times').addClass('fa-bars');
          $floatNav.addClass('closed');
          $floatNav.find('table').addClass('hidden');
        }, 200);
      },
    },
    ronin: {
      checkStatus: function () {
        if ($.cookie('sid') && $.cookie('sid').length != 192) $.removeCookie('sid');
        $('#ronin_write').prop('checked', ($.cookie('use_ronin') == 'true'));
        if ($.cookie('ronin') == 'true') {
          $('.roninform_logined').removeClass('hidden');
          $('.roninform_unlogin').addClass('hidden');
        } else {
          $('.roninform_unlogin').removeClass('hidden');
          $('.roninform_logined').addClass('hidden');
        }
      },
      isLogin: function () {
        return ($.cookie('ronin') === 'true');
      }
    },
    addEvent: {
      clickEventTracker: function (page) {
        switch (page) {
        case 'thread':
          $(document).on('click', '.quotes_body .subback a', function () {
            _5ch.eventTrack('各板で今人気のスレッド', 'クリック', this.children[1].innerText);
          });
          break;
        default:
        }
        return this;
      },
      messageHelper: function () {
        $('.js-message_helper').on('click', function () {
          var $message = $('#js-postform_message');
          var message = $message.val();
          switch ($(this).prop('id').replace('js-message_helper-', '')) {
          case ('sage'):
            $('#postform_emailfield').val('sage');
            break;
          case ('clear'):
            setTimeout(function () {
              if (window.confirm('削除してもよろしいですか?')) $message.val('');
            }, 10);
            break;
          case ('oekaki'):
            $.getScript("/assets/js/oekaki/5ch.js", function () {
              oekaki.init();
            });
            $(this).remove();
            break;
          }
        });
        return this;
      },
      logoutRonin: function () {
        $('#ronin_logout').on('click', function () {
          if (window.confirm('ログアウトしようとしてます。よろしいですか?')) {
            $.cookie('ronin', false, {
              expires: 1,
              path: '/'
            });
            $('#ronin_write').prop('checked', false);
            _5ch.ronin.checkStatus();
            $.ajax({
              url: '/ronin/logout',
              timeout: 1000,
              success: function (result, textStatus, xhr) {
                if (result) {
                  alert('ログアウトしました。');
                } else {
                  alert('正常にログインできていない可能性があります。再度ログインしてください。');
                }
              },
              error: function (xhr, textStatus, error) {
                alert('通信に失敗しました');
              }
            });
          }
        });
        return this;
      },
      UseRoninCheckbox: function () {
        $('#ronin_write').on('click', function () {
          if ($(this).prop('checked')) {
            if ($.cookie('ronin') != 'true') {
              $('html,body').animate({
                scrollTop: $('#js-ronin_email').offset().top
              }, 400);
              $('#js-ronin_email').focus();
              return false;
            }
            $.ajax({
              url: '/ronin/sid',
              timeout: 5000,
              beforeSend: function (xhr, settings) {
                $('.js-postform_submit').attr('disabled', true);
              },
              complete: function (xhr, textStatus) {
                $('.js-postform_submit').attr('disabled', false);
              },
              success: function (result, textStatus, xhr) {
                if (!result) {
                  alert('正常にログインできていない可能性があります。再度ログインしてください。');
                  $('#ronin_write').prop('checked', false);
                  $.cookie('ronin', false, {
                    expires: 1,
                    path: '/'
                  });
                  _5ch.ronin.checkStatus();
                  return false;
                }
                $.cookie('use_ronin', true, {
                  expires: 1,
                  path: '/'
                });
              },
              error: function (xhr, textStatus, error) {
                $('#ronin_write').prop('checked', false);
                alert('通信に失敗しました');
              }
            });
          } else {
            $.ajax({
              url: '/ronin/unsetsid',
              timeout: 5000,
              success: function (result, textStatus, xhr) {
                console.log(result);
                $.cookie('use_ronin', false, {
                  expires: 1,
                  path: '/'
                });
              },
              error: function (xhr, textStatus, error) {
                alert('通信に失敗しました');
              }
            });
          }
        });
        return this;
      },
      loginRonin: function () {
        $('#ronin_login').submit(function (e) {
          e.preventDefault();
          var $form = $(this);
          var $button = $form.find('.postform_input-submit');
          $.ajax({
            url: $form.attr('action'),
            type: $form.attr('method'),
            data: $form.serialize(),
            timeout: 10000,
            beforeSend: function (xhr, settings) {
              $button.attr('disabled', true).val('ログイン中...');
            },
            complete: function (xhr, textStatus) {
              $button.attr('disabled', false).val('ログインする');
            },
            success: function (result, textStatus, xhr) {
              if (!result) {
                alert('認証に失敗しました');
                return false;
              }
              $.cookie('ronin', true, {
                expires: 1,
                path: '/'
              });
              alert('認証に成功しました');
              _5ch.ronin.checkStatus();
            },
            error: function (xhr, textStatus, error) {
              alert('認証に失敗しました');
            }
          });
        });
        return this;
      },
      go_writeform: function () {
        var $form = $('#js-forms');
        $(document).on('blur, focus', '#js-forms input, #js-forms textarea', function () {
          setTimeout(function () {
            $form.css({
              'top': $(document).scrollTop() + 'px',
              'height': screen.availHeight + 'px'
            });
          }, 100);
        });
        $(document).on('click', '.js-go_writeform', function () {
          $form.addClass('hidden');
          $form.find('.form_title').addClass('hidden');
          $form.find('.header').removeClass('hidden');
          $form.css({
            'top': $(document).scrollTop() + 'px',
            'height': screen.availHeight + 'px'
          });
          $form.addClass('forms-window forms-window-translated');
          setTimeout(function () {
            $form.removeClass('hidden');
          }, 300);
          setTimeout(function () {
            $form.removeClass('forms-window-translated');
          }, 350);
          _5ch.navButton.hide();
        });
        return this;
      },
      closeWriteFormWindow: function () {
        $('#js-write-window-close').click(function () {
          $('#js-forms').addClass('forms-window-translated');
          $('#blackbox').trigger('click');
          setTimeout(function () {
            var $form = $('#js-forms');
            $form.attr('style', '');
            $form.removeClass('forms-window forms-window-translated');
            $form.find('.form_title').removeClass('hidden');
            $form.find('.header').addClass('hidden');
          }, 300);
        });
        return this;
      },
      gesture_to_move: function (target_name) {
        if (!target_name) return this;
        var firstY = 0;
        var lastY = 0;
        var timer = 0;
        var direction = '';
        var $t = $('#thread');
        $t.on('touchstart', function (e) {
          if (e.originalEvent.touches.length != 2) {
            return;
          }
          firstY = e.originalEvent.touches ? e.originalEvent.touches[0].screenY : e.screenY;
        });
        $t.on('touchmove', function (e) {
          if (e.originalEvent.touches.length != 2) {
            return;
          }
          e.preventDefault();
          clearTimeout(timer);
          var currentY = e.originalEvent.touches ? e.originalEvent.touches[0].screenY : e.screenY;
          if (lastY === 0) lastY = currentY;
          if (Math.abs(currentY - firstY) < 10) {
            $('#moveUpLeader').addClass('moveLeader-bottom-traslated');
            $('#moveDownLeader').addClass('moveLeader-top-traslated');
            direction = '';
            return;
          }
          direction = (lastY - firstY) > 0 ? 'up' : 'down';
          switch (direction) {
          case ('up'):
            $('#moveDownLeader').addClass('moveLeader-top-traslated');
            $('#moveUpLeader').removeClass('moveLeader-bottom-traslated');
            break;
          case ('down'):
            $('#moveUpLeader').addClass('moveLeader-bottom-traslated');
            $('#moveDownLeader').removeClass('moveLeader-top-traslated');
            break;
          }
          lastY = currentY;
          timer = setTimeout(function () {
            lastY = 0;
          }, 200);
        });
        $t.on('touchend', function (e) {
          firstY = 0;
          lastY = 0;
          if (direction === '') return;
          var $reload = $('#reload_thread');
          var height = window.innerHeight ? window.innerHeight : $(window).height;
          var scrollTop = $reload.offset().top;
          var doc_height = (scrollTop - height + $reload.outerHeight());
          var base_position = (direction === 'up') ? 0 : doc_height;
          var diff_scroll = Math.abs(base_position - document.body.scrollTop);
          if (diff_scroll > 4000) {
            if (direction === 'up') window.scrollTo(0, 4000);
            if (direction === 'down') window.scrollTo(0, doc_height - 4000);
          }
          $('html,body').animate({
            scrollTop: direction == 'up' ? 0 : doc_height
          }, 500);
          direction = '';
          $('#moveUpLeader').addClass('moveLeader-bottom-traslated');
          $('#moveDownLeader').addClass('moveLeader-top-traslated');
        });
        return this;
      },
      toggle_header: function (target_name) {
        var target_name = 'body';
        var start = 0;
        setInterval(function () {
          start = $(document).scrollTop();
        }, 300);
        setInterval(function () {
          var distanceY = $(document).scrollTop() - start;
          if (distanceY > 20) {
            $('#header').removeClass('header-fixed').addClass('header-absolute');
          } else if (-10 > distanceY) {
            $('#header').removeClass('header-absolute').addClass('header-fixed');
          }
        }, 200);
        return this;
      },
      viewerSetting: function () {
        $('.js-pcurl').bind('click', function (e) {
          var keep = '';
          if (window.confirm("PC表示を3日間保持し続けますか?\nOKを選ぶと3日間保持、\nキャンセルを選ぶと今だけ遷移します。\n\n保持状態の変更は、このボタンの右にある'設定'より変更できます。")) {
            $.cookie('viewer', 'pc', {
              expires: 3,
              path: '/'
            });
            keep = '3日間保持';
          } else {
            keep = '保持無し';
          }
          _5ch.eventTrack('表示設定', 'クリック', 'PC版で表示#' + keep);
        });
        return this;
      },
      stop_submit: function (fn) {
        $(document).on('submit', '#search_form', function (e) {
          $('#search_input_text').blur();
          if (fn) fn();
          return false;
        });
        return this;
      },
    },
    Ad: {
      use_lazy_load: false,
      lazy_load_interval: null,
      downloaded: {
        data: [],
        at: 0
      },
      tags: {
        content: {
          top_news_pc_1: '<iframe src="//stab.thench.net/itest/top_news_pc_728x90_1"    style="width:728px;height: 90px; margin: 0px auto;display: block;border: 0;"></iframe>',
          top_news_pc_2: '<iframe src="//stab.thench.net/itest/top_news_pc_728x90_2"    style="width:728px;height: 90px; margin: 0px auto;display: block;border: 0;"></iframe>',
          top_news_pc_3: '<iframe src="//stab.thench.net/itest/top_news_pc_728x90_3"    style="width:728px;height: 90px; margin: 0px auto;display: block;border: 0;"></iframe>',
          top_popular_pc_1: '<iframe src="//stab.thench.net/itest/top_popular_728x90_1"    style="width:728px;height: 90px; margin: 0px auto;display: block;border: 0;"></iframe>',
          top_popular_pc_2: '<iframe src="//stab.thench.net/itest/top_popular_728x90_2"    style="width:728px;height: 90px; margin: 0px auto;display: block;border: 0;"></iframe>',
          top_popular_pc_3: '<iframe src="//stab.thench.net/itest/top_popular_728x90_3"    style="width:728px;height: 90px; margin: 0px auto;display: block;border: 0;"></iframe>',
          top_rectangle_pc_1: '<iframe src="//stab.thench.net/itest/top_rectangle_300x250_1" style="width:300px;height: 250px;margin: 0px auto;display: block;border: 0;"></iframe>',
          top_rectangle_pc_2: '<iframe src="//stab.thench.net/itest/top_rectangle_300x250_2" style="width:300px;height: 250px;margin: 0px auto;display: block;border: 0;"></iframe>',
          top_rectangle_pc_3: '<iframe src="//stab.thench.net/itest/top_rectangle_300x250_3" style="width:300px;height: 250px;margin: 0px auto;display: block;border: 0;"></iframe>',
          top_rectangle_pc_4: '<iframe src="//stab.thench.net/itest/top_rectangle_300x250_4" style="width:300px;height: 250px;margin: 0px auto;display: block;border: 0;"></iframe>',
          top_news: ['<script type="text/javascript">', '  var nend_params = {"media":17755,"site":236852,"spot":812293,"type":10,"oriented":1};', '</script>', '<script type="text/javascript" src="https://js1.nend.net/js/nendAdLoader.js"></script>'].join(''),
          top_popular: ['<script type="text/javascript">', '  var nend_params = {"media":17755,"site":236852,"spot":812295,"type":10,"oriented":1};', '</script>', '<script type="text/javascript" src="https://js1.nend.net/js/nendAdLoader.js"></script>'].join(''),
          top_rectangle: ['<script src="//i2ad.jp/customer/gmn/itest.php?id=5"></script>'].join(''),
        },
        subback: {
          bottom_300x250: ['<script type="text/javascript">', 'var adstir_vars = {', '  ver: "4.0",', '  app_id: "MEDIA-8ccac345",', '  ad_spot: 79,', '  center: false', '};', '</script>', '<script type="text/javascript" src="https://js.ad-stir.com/js/adstir.js"></script>'].join(''),
          first_320x180_1: ['<script async src="//cdn.fivecdm.com/adsbyfive.js"></script>', '<ins class="adsbyfive" style="display: none;" data-media-id="841586" data-slot-id="399099"></ins>', '<script>(adsbyfive = window.adsbyfive || []).push({});</script>'].join(''),
          third_320x180: ['<script src="//i2ad.jp/customer/gmn/itest.php?id=7"></script>'].join(''),
          infeed_320x100: '<script src="//i2ad.jp/customer/cn2lech/2ch7.php?id=8"></script>',
          native_ad_1: ['<script type="text/javascript">', 'var microadCompass = microadCompass || {};', 'microadCompass.queue = microadCompass.queue || [];', '</script>', '<script type="text/javascript" charset="UTF-8" src="//j.microad.net/js/compass.js" onload="new microadCompass.AdInitializer().initialize();" async></script>', '<div id="5d5572407dabd14455d18f194502d3ea" >', '  <script type="text/javascript">', '    microadCompass.queue.push({', '      "spot": "5d5572407dabd14455d18f194502d3ea"\,', '      "url": "${COMPASS_EXT_URL}"\,', '      "referrer": "${COMPASS_EXT_REF}"', '    });', '  </script>', '</div>'].join(''),
          native_ad_2: ['<script type="text/javascript">', 'var microadCompass = microadCompass || {};', 'microadCompass.queue = microadCompass.queue || [];', '</script>', '<script type="text/javascript" charset="UTF-8" src="//j.microad.net/js/compass.js" onload="new microadCompass.AdInitializer().initialize();" async></script>', '<div id="bd95c47f7792c746d70feee454581218" >', '  <script type="text/javascript">', '    microadCompass.queue.push({', '      "spot": "bd95c47f7792c746d70feee454581218",', '      "url": "${COMPASS_EXT_URL}",', '      "referrer": "${COMPASS_EXT_REF}"', '    });', '  </script>', '</div>'].join(''),
          native_ad_3: ['<script type="text/javascript">', 'var microadCompass = microadCompass || {};', 'microadCompass.queue = microadCompass.queue || [];', '</script>', '<script type="text/javascript" charset="UTF-8" src="//j.microad.net/js/compass.js" onload="new microadCompass.AdInitializer().initialize();" async></script>', '<div id="97aeceb4545fcf0d4892f2685b327ea7" >', '  <script type="text/javascript">', '    microadCompass.queue.push({', '      "spot": "97aeceb4545fcf0d4892f2685b327ea7",', '      "url": "${COMPASS_EXT_URL}",', '      "referrer": "${COMPASS_EXT_REF}"', '    });', '  </script>', '</div>'].join(''),
          native_ad_4: ['<script type="text/javascript">', 'var microadCompass = microadCompass || {};', 'microadCompass.queue = microadCompass.queue || [];', '</script>', '<script type="text/javascript" charset="UTF-8" src="//j.microad.net/js/compass.js" onload="new microadCompass.AdInitializer().initialize();" async></script>', '<div id="1ce185aa563a108e0b6a57f5582d7aa4" >', '  <script type="text/javascript">', '    microadCompass.queue.push({', '      "spot": "1ce185aa563a108e0b6a57f5582d7aa4",', '      "url": "${COMPASS_EXT_URL}",', '      "referrer": "${COMPASS_EXT_REF}"', '    });', '  </script>', '</div>', ].join(''),
        },
        thread: {
          header_320x50_1: ['<script type="text/javascript" src="https://js.sprout-ad.com/t/238/896/a1238896.js"></script>'].join(''),
          header_320x50_2: ["<script type=text/javascript src='//i2ad.jp/i/iVCCp0kwKCl4/sync'></script>", "<script type=text/javascript>_iads.outAd(160);</script>"].join(''),
          first_320x180_1: ['<script async src="//cdn.fivecdm.com/adsbyfive.js"></script>', '<ins class="adsbyfive" style="display: none;" data-media-id="841586" data-slot-id="399099"></ins>', '<script>(adsbyfive = window.adsbyfive || []).push({});</script>'].join(''),
          cardview_320x180: ['<script src="//i2ad.jp/customer/cn2lech/5ch.php?id=2"></script>'].join(''),
        },
        find: {
          top_320x50: ['<script type="text/javascript">', 'var adstir_vars = {', '  ver: "4.0",', '  app_id: "MEDIA-8ccac345",', '  ad_spot: 81,', '  center: false', '};', '</script>', '<script type="text/javascript" src="https://js.ad-stir.com/js/adstir.js"></script>'].join(''),
          bottom_300x250: ['<script type="text/javascript">', 'var adstir_vars = {', '  ver: "4.0",', '  app_id: "MEDIA-8ccac345",', '  ad_spot: 83,', '  center: false', '};', '</script>', '<script type="text/javascript" src="https://js.ad-stir.com/js/adstir.js"></script>'].join(''),
        },
        histories: {
          top_320x50: ['<script type="text/javascript">', 'var adstir_vars = {', '  ver: "4.0",', '  app_id: "MEDIA-8ccac345",', '  ad_spot: 84,', '  center: false', '};', '</script>', '<script type="text/javascript" src="https://js.ad-stir.com/js/adstir.js"></script>'].join(''),
          bottom_300x250: ['<script type="text/javascript">', 'var adstir_vars = {', '  ver: "4.0",', '  app_id: "MEDIA-8ccac345",', '  ad_spot: 85,', '  center: false', '};', '</script>', '<script type="text/javascript" src="https://js.ad-stir.com/js/adstir.js"></script>'].join(''),
        },
      },
      show: function (target_dom) {
        var div = target_dom;
        var ad = _5ch.Ad.random_js_ad();
        if (!ad) {
          $(target_dom).closest('.res_ad').remove();
          return false;
        }
        var style = {
          "width": 320,
          "height": 250,
          "sandbox": false
        };
        if (div.classList.contains('js-movie') && _5ch.env.site_type === '5ch' && _5ch.env.controller === 'subback') {
          style.height = 180;
          ad.hash = 'movie';
          ad.body = _5ch.Ad.tags.subback['first_320x180_1'];
        }
        if (div.classList.contains('js-third') && _5ch.env.site_type === '5ch') {
          style.height = 250;
          ad.hash = '3rd';
          ad.body = _5ch.Ad.tags.subback.third_320x180;
        }
        if (div.classList.contains('js-infeed') && _5ch.env.site_type === '5ch') {
          style.height = 100;
          ad.hash = 'infeed';
          ad.body = _5ch.Ad.tags.subback.infeed_320x100;
        }
        if (div.classList.contains('js-five') && _5ch.env.site_type === '5ch') {
          style.height = 180;
          ad.hash = 'five';
          ad.body = _5ch.Ad.tags.thread['first_320x180_1'];
        }
        if (div.classList.contains('js-subback-native') && _5ch.env.site_type === '5ch') {
          var i = 1;
          ad.hash = 'subback-native';
          while (5 > i) {
            if (_5ch.Ad.tags.subback["native_ad_" + i] != '') {
              ad.body = _5ch.Ad.tags.subback["native_ad_" + i];
              style.width = '100%';
              style.height = 78;
              _5ch.Ad.tags.subback["native_ad_" + i] = '';
              break;
            }
            i++;
          }
        }
        ad.body = _5ch.Ad.tagSeparator(ad.body);
        var html = "<body style='margin: 0;' data-itestadid='" + ad.hash + "'>" + ad.body + "</body>";
        _5ch.dom.addIframe(div, html, style.width, style.height, 'js-ad_iframe ad_network_content', style.sandbox);
      },
      tagSeparator: function (body) {
        if (!body) return "";
        if (body.toLowerCase().indexOf('<ssl>') == -1) return body;
        if (location.protocol === 'http:') body = body.match(/<NONSSL>([\s\S]+)<\/NONSSL>/)[1];
        if (location.protocol === 'https:') body = body.match(/<SSL>([\s\S]+)<\/SSL>/)[1];
        return body;
      },
      watchStatusOfAdNetworkView: function () {
        if (this.use_lazy_load == false) return false;
        if (this.lazy_load_interval != null) return true;
        this.lazy_load_interval = setInterval(function () {
          var $js_ad_iframe_wraps = $('.js-ad_iframe_wrap.js-disable .lazy-image[src="/assets/img/loading.gif"]');
          if ($js_ad_iframe_wraps.length === 0) return;
          $js_ad_iframe_wraps.each(function (k, div) {
            var $parent = $(div).parent();
            if ($parent.hasClass('js-disable')) {
              $parent.removeClass('js-disable');
            } else {
              return true;
            }
            _5ch.Ad.show(div.parentNode);
            $parent.find('img').empty();
          });
        }, 500);
      },
      data: function (type) {
        if (!type) return _5ch.Ad.downloaded.data;
        return $.grep(_5ch.Ad.downloaded.data, function (v, k) {
          return v.adType === type;
        });
      },
      check_data: function () {
        if (_5ch.Ad.downloaded.data == null) return false;
        return _5ch.Ad.downloaded.data.length > 0 ? true : false;
      },
      random: function (ads) {
        if (!ads) ads = _5ch.Ad.data();
        if (!ads) return false;
        if (0 >= ads.length) return false;
        var random = Math.floor(Math.random() * ads.length);
        var ad = ads[random];
        _5ch.Ad.downloaded.data = ads;
        return ad;
      },
      random_js_ad: function () {
        var js_ads = this.data('js');
        if (!js_ads) return false;
        if (0 >= js_ads.length) return false;
        var ads = this.data();
        var random = Math.floor(Math.random() * js_ads.length);
        var js_ad = js_ads[random];
        var remove_key = null;
        $.each(ads, function (k, ad) {
          if (ad.hash === js_ad.hash) {
            remove_key = k;
            return false;
          }
        });
        ads.splice(remove_key, 1);
        _5ch.Ad.downloaded.data = ads;
        return js_ad;
      },
      onclick: function (className) {
        $(className).find('a').on('click', function () {
          window.open(itest5ch.env.location_origin.replace('itest.', 'iand') + '/' + $(this).attr('href'));
          return false;
        });
      },
      network: function () {
        $(".lazy-image").lazyload({
          threshold: 1000
        }).removeClass('.lazy-image');
        if (this.use_lazy_load) {
          this.watchStatusOfAdNetworkView();
          return false;
        }
        var $js_ad_iframe_wraps = $('.js-ad_iframe_wrap');
        if ($js_ad_iframe_wraps.length === 0) return;
        if (_5ch.env.pc) {
          var ads = [{
            name: 'top_news',
            height: 100,
            ads_count: 3
          }, {
            name: 'top_popular',
            height: 100,
            ads_count: 3
          }, {
            name: 'top_rectangle',
            height: 250,
            ads_count: 4
          }];
          $js_ad_iframe_wraps.each(function (k, div) {
            if ($(div).hasClass('js-disable')) {
              $(div).removeClass('js-disable').empty();
            } else {
              return true;
            }
            if ($(div).is('.js-third,.js-infeed')) $(div).removeClass('js-third js-infeed');
            if ($(div).is('.js-movie,.js-subback-native')) {
              $(div).closest('li.ad').remove();
              return true;
            }
            if (_5ch.env.pc_ads_count >= _5ch.env.max_pc_ads_count) {
              $(div).closest('li.ad').remove();
              return true;
            }
            _5ch.env.pc_ads_count++;
            setTimeout(function () {
              var n = _5ch.number.random(ads.length);
              var ad = ads[n - 1];
              var file_name = ad.name + '_pc_' + _5ch.number.random(ad.ads_count);
              _5ch.dom.addIframe(div, _5ch.Ad.tags.content[file_name], '100%', ad.height);
            }, k * 200);
          });
        } else {
          $js_ad_iframe_wraps.each(function (k, div) {
            if ($(div).hasClass('js-disable')) {
              $(div).removeClass('js-disable').empty();
            } else {
              return true;
            }
            setTimeout(function () {
              _5ch.Ad.show(div);
            }, k * 200);
          });
        }
        return false;
      },
      toppage: {
        to_html: function (ad, target) {
          if (!ad) return '';
          switch (target) {
          case 'ranking':
            if (ad.adType === 'js') return '<div class="js-ad_iframe_wrap js-disable"><img src="/assets/img/loading.gif"></div>';
            var html = '';
            html += '<a href="' + _5ch.env.ad_location_origin + '/ad/' + ad.hash + '/' + _5ch.text.rand() + '" target="_blank" class="' + ad.adType + '">';
            html += ad.body;
            html += '</a>';
            return html;
            break;
          }
          return '';
        }
      },
      Subback: {
        to_html: function (ad, index) {
          if (!ad) return '';
          if (_5ch.env.site_type === '5ch' && document.getElementsByClassName('ad').length == 0) {
            if (_5ch.Subback.infeed_count === 0 || _5ch.Subback.infeed_count === 2) {
              var class_name = '';
              if (_5ch.Subback.infeed_count === 0 && _5ch.env.controller === 'subback') class_name = 'js-movie';
              if (_5ch.Subback.infeed_count === 2) class_name = 'js-third';
              _5ch.Subback.infeed_count++;
              return '<div class="js-ad_iframe_wrap js-disable ' + class_name + '">' + '<img src="/assets/img/loading.gif">' + '</div>';
            }
            if (3 >= _5ch.Subback.infeed_count) {
              _5ch.Subback.infeed_count++;
              return '<div class="js-ad_iframe_wrap js-disable js-infeed"><img src="/assets/img/loading.gif"></div>';
            }
          }
          if (ad.adType === 'js') return '<div class="js-ad_iframe_wrap js-disable"><img src="/assets/img/loading.gif"></div>';
          var html = '';
          html += '<a href="' + _5ch.env.ad_location_origin + '/ad/' + ad.hash + '/' + _5ch.text.rand() + '" target="_blank" class="' + ad.adType + '">';
          html += '<span class="info">';
          if (ad.adType === 'text') {
            html += '<span>' + _5ch.time.unix_to_5chformat(_5ch.time.unix()) + '</span>';
            html += '<span class="ikioi b">【PR】</span>';
          }
          html += '</span>';
          html += ad.body;
          html += '</a>';
          return html;
        }
      },
      Thread: {
        to_html: function (ad, index) {
          if (!ad) return '';
          if (_5ch.env.site_type === '5ch' && document.getElementsByClassName('res_ad').length == 0) {
            if (0 === _5ch.Thread.infeed_count) {
              _5ch.Thread.infeed_count++;
              return '<div class="js-ad_iframe_wrap js-disable js-five"><img class="lazy-image" data-original="/assets/img/loading.gif"></div>';
            }
          }
          if (ad.adType === 'js') return '<div class="js-ad_iframe_wrap js-disable"><img src="/assets/img/loading.gif"></div>';
          var html = '';
          if (ad.adType == 'text') {
            html += '<div class="threadview_response_info">【PR】 広告＠転載は禁止 ' + _5ch.time.unix_to_5chformat(_5ch.time.unix()) + '</div>';
            html += '<div class="threadview_response_body">';
            html += '<a href="' + _5ch.env.ad_location_origin + '/ad/' + ad.hash + '/' + _5ch.text.rand() + '" target="_blank" class="threadview_response_body_link">' + ad.domain + '</a><br/>';
            html += ad.body;
            html += '</div>';
            html += '<div class="threadview_response_detail">【PR】</div>';
          } else {
            html += '<a href="' + _5ch.env.ad_location_origin + '/ad/' + ad.hash + '/' + _5ch.text.rand() + '" target="_blank" class="threadview_response_body_link">';
            html += '<div class="body">' + ad.body + '</div>';
            html += '</a>';
          }
          return html;
        }
      },
      download: function (params) {
        var url = _5ch.env.location_origin + '/rect/' + _5ch.env.board_type + '.json';
        return $.ajax({
          type: 'GET',
          url: url,
          data: params,
          success: function (response) {
            if ($.present(response)) {
              _5ch.Ad.downloaded.data = _5ch.JSON.check(response) ? response : JSON.parse(response);
            }
          }
        });
      },
    },
    Bbsmenu: {
      addEvent: {
        searchThread: function () {
          $("#search_thread").click(function () {
            if (!$('#search_input_text').val()) {
              alert('検索ワードを入力してください');
              return false;
            }
            $(this).closest('form').submit();
            return false;
          });
          return this;
        },
        searchBoard: function () {
          $('#search_board').click(function () {
            var q = $('#search_board_input_text').val();
            if (!q) {
              _5ch.Bbsmenu.clearSearchHistory();
              return false;
            }
            var hits = $.grep($('.board_list .pure-menu-link-board'), function (v, k) {
              return v.text.indexOf(q) >= 0;
            });
            _5ch.Bbsmenu.clearSearchHistory();
            $('#js-show_searching_board').removeClass('hidden').find('.pure-menu-list').append('<li class="pure-menu-item pure-menu-selected">' + '<a href="#" class="pure-menu-link">' + '<strong>検索ワードを含む板</strong>' + '</a>' + '</li>');
            $.each(hits, function (k, v) {
              $('#js-show_searching_board').find('.pure-menu-list').append($('<li>').addClass('pure-menu-item').append('<li class="pure-menu-item">' + '<a href="' + v.href + '" class="pure-menu-link">' + v.text + '</a>' + '</li>'));
            });
            return false;
          });
          return this;
        },
        showBoards: function (fn) {
          $('.js-show-boards').click(function () {
            $('.board_list').addClass('board_list-fixed hidden');
            $('.board_list_closer').removeClass('hidden');
            setTimeout(function () {
              $('.board_list').removeClass('hidden');
            }, 20);
            setTimeout(function () {
              $('.board_list').removeClass('traslated');
              if (fn) fn();
            }, 40);
            return false;
          });
          return this;
        },
        closeBoards: function (fn) {
          $('#js-close-boards').click(function () {
            $('.board_list').addClass('traslated');
            setTimeout(function () {
              $('.board_list').removeClass('board_list-fixed');
              $('.board_list_closer').addClass('hidden');
              _5ch.Bbsmenu.clearSearchHistory();
            }, 300);
            if (fn) fn();
            return false;
          });
          return this;
        },
        toggleCategory: function () {
          $(document).on('click', '.pure-menu-selected .pure-menu-link', function (e) {
            $('.js-category-id-' + $(this).data('categoryId')).toggleClass('pure-menu-item-none');
            return false;
          });
          return this;
        }
      },
      clearSearchHistory: function () {
        $('#js-show_searching_board').addClass('hidden').find('.pure-menu-list').empty();
      },
      yokumiru: function () {
        var load_db = function () {
          var db = _5ch.db.load();
          return db ? db.query('subbacks') : false;
        };
        var boards = $('#history');
        var history = load_db();
        if (history === false) return this;
        if (history.length === 0) return this;
        boards.empty();
        var sorted = _5ch.db.Subback.access_count_sort(history);
        var html = '<li class="pure-menu-item pure-menu-selected">' + '<a href="#" class="pure-menu-link highlight">' + '<strong>よく見る板</strong>' + '</a>' + '</li>';
        $(sorted).each(function (k, v) {
          html += '<li class="pure-menu-item">' + '<a href="/subback/' + v.board + '" class="pure-menu-link">' + v.name + '</a>' + '</li>';
        });
        boards.append(html);
        return this;
      },
      contentAds: function () {
        $.each([{
          name: 'top_news',
          height: 100,
          ads_count: 3
        }, {
          name: 'top_popular',
          height: 100,
          ads_count: 3
        }, {
          name: 'top_rectangle',
          height: 250,
          ads_count: 3
        }], function (k, h) {
          var file_name = h.name;
          if (_5ch.env.pc) file_name += '_pc_' + _5ch.number.random(h.ads_count);
          $.each(document.querySelectorAll('.js-ads-content-' + h.name), function (k, v) {
            _5ch.dom.addIframe(v, _5ch.Ad.tags.content[file_name], '100%', h.height);
          });
        });
        return this;
      },
      thread_histories: function (all) {
        var db = _5ch.db.load();
        var load_db = function () {
          return db ? db.query('threads') : false;
        };
        var threads = $('#thread_histories');
        var history = load_db();
        if (history === false) return this;
        if (history.length === 0) return this;
        var subbacks = {};
        var _subbacks = db.query('subbacks');
        $.each(_subbacks, function (k, v) {
          subbacks[v.board] = v;
        });
        threads.empty();
        history = history.sort(function (a, b) {
          return (a.updated_at < b.updated_at) ? 1 : -1;
        });
        var html = '';
        var limiter = all ? 200 : 10;
        $(history).each(function (k, v) {
          if (k >= limiter) {
            if (all === false) {
              html += '<li class="news">' + '  <div class="news_link text-center">' + '    <a href="/histories">閲覧履歴をもっと見る<i class="fa fa-angle-down"></i></a>' + '  </div>' + '</li>';
            }
            return false;
          };
          var subback = subbacks[v.board];
          html += '<li class="news">' + '<div class="news_link">' + '<a href="/' + v.subdomain.split('.')[0] + "/test/read.cgi/" + v.board + "/" + v.dat + '">' + '<div class="news_box news_box-right">' + '<div class="news_title">' + v.title + '</div>' + '<div class="news_detail text-right">';
          if (subback) html += '<span class="news_detail_child">' + subbacks[v.board].name + '</span>';
          html += '<span class="news_detail_child news_detail_child-ikioi">' + 'レス数:<span class="highlight"><i class="fa fa-comments"></i>' + v.response_count + '</span>' + '</span>' + '</div>' + '</div>' + '</a>' + '</div>' + '</li>';
        });
        threads.append(html);
        return this;
      },
    },
    Subback: {
      infeed_count: 0,
      downloaded: {
        data: {},
        at: 0
      },
      addEvent: {
        choiceArea: function () {
          var settings = {
            subback: "#subback"
          };
          var $subback = $(settings.subback);
          var fn = false;
          $('#js-areas').change(function () {
            var area = $(this).val();
            if (area === '0') {
              _5ch.Subback.show(function () {
                _5ch.Subback.change_paging_status();
                $.addClasses({
                  '#loading_bar': 'hidden',
                  '#loading_bar_bottom': 'hidden'
                });
                $.removeClasses({
                  '#paging': 'hidden',
                  '#paging_bottom': 'hidden',
                  '#subback': 'subback-choicedarea'
                });
              });
              return false;
            }
            _5ch.eventTrack('地域選択', area, _5ch.request.board);
            var params = {
              q: area
            };
            if (_5ch.request.board) {
              params.board = _5ch.request.board;
            }
            $.when(_5ch.Search.download(params, true)).done(function (downloaded) {
              if (0 >= _5ch.Search.downloaded.data.total_count || !_5ch.Search.downloaded.data) {
                $('#subback').empty();
                $subback.append('<li><p class="disconnect">' + area + 'を含むスレッドはありませんでした。</p></li>');
                $.addClasses({
                  '#paging': 'hidden',
                  '#paging_bottom': 'hidden',
                  '#subback': 'subback-choicedarea'
                });
                return false;
              }
              $('#subback').removeClass('hidden');
              _5ch.Search.render('#subback-tmpl', settings.subback, function () {
                _5ch.Ad.network();
                $.addClasses({
                  '#paging': 'hidden',
                  '#paging_bottom': 'hidden',
                  '#subback': 'subback-choicedarea'
                });
              });
              if (fn) fn();
            }).fail(function () {
              $('#subback').html(_5ch.error_messages.subback.disconnect);
            });
          });
          return this;
        },
        moreSubback: function () {
          $('#subback').on('click', '#more_subback', function () {
            $(this).html('読み込み中<i class="fa fa-refresh fa-pulse"></i>');
            var self = this;
            setTimeout(function () {
              $(self).closest('li').remove();
              $('#instant_ad').remove();
              itest5ch.Subback.render('#subback-tmpl', '#subback');
            }, 100);
            return false;
          });
          return this;
        },
        changeCharset: function () {
          $('.postform').submit(function () {
            document.charset = 'Shift-JIS';
            if ($.cookie('track-new-thread-button') != _5ch.request.board) {
              $.cookie('track-new-thread-button', _5ch.request.board, {
                expires: 1,
                path: '/'
              });
              _5ch.eventTrack('スレッドを立てるボタン', 'クリック', _5ch.request.board);
            }
          });
          return this;
        },
        reload: function (fn) {
          var status = {
            is_loading: false,
            self: null,
            backup_html: null,
            check_download_interval: function () {
              return (10 > (_5ch.time.unix() - _5ch.Subback.downloaded.at));
            },
            return_back: function () {
              setTimeout(function () {
                $(status.self).html(status.backup_html);
                $('#blackbox').trigger('click');
                _5ch.scroll.top();
                _5ch.navButton.hide();
              }, 300);
              status.is_loading = false;
            }
          };
          $('.reload').bind('click', function (e) {
            status.self = this;
            status.backup_html = $(this).html();
            if (status.is_loading) return false;
            status.is_loading = true;
            $(status.self).html('<i class="fa fa-refresh fa-spin"></i>');
            if (status.check_download_interval()) {
              status.return_back();
              return false;
            }
            _5ch.Subback.download({
              board: _5ch.request.board
            }).done(function (downloaded) {
              $('#subback').empty();
              _5ch.Subback.render('#subback-tmpl', '#subback');
              status.return_back();
            }).fail(function () {
              alert('ダウンロードに失敗しました。時間を置いて再度お試しください。');
            });
          });
          return this;
        },
        sort: function (fn) {
          $('.sort').bind('click', function () {
            var defaultValue = $(this).html(),
              _self = this,
              target = 'normal',
              data = {},
              sorting_html = '<i class="fa fa-refresh fa-pulse"></i><span>ソート中</span>';
            if (defaultValue == sorting_html) return false;
            $(_self).html(sorting_html);
            target = $(_self).attr('class').match(/sort_(.+)/)[1];
            _5ch.Subback.downloaded.data.threads = _5ch.Subback.sortBy(target);
            $('#subback').empty();
            _5ch.Subback.render('#subback-tmpl', '#subback', function () {
              setTimeout(function () {
                $('#blackbox').trigger('click');
                _self.innerHTML = defaultValue;
                _5ch.scroll.top();
                _5ch.Ad.network();
              }, 300);
              if (fn) fn();
              _5ch.navButton.hide();
              $('.tabs-navigation .tabs-navigation-body[href=#menu]').click();
            });
          });
          return this;
        },
        floatNav: function () {
          var floatNavToggle = $('.float-nav .toggle');
          floatNavToggle.on('click', function (e) {
            var $nav = $(this).closest('nav');
            var $table = $nav.find('table');
            $nav.toggleClass('closed');
            $table.toggleClass('hidden');
            if ($nav.hasClass('closed')) {
              floatNavToggle.find('.fa.fa-times').removeClass('fa-times').addClass('fa-bars');
              $('#blackbox').hide();
            } else {
              floatNavToggle.find('.fa.fa-bars').removeClass('fa-bars').addClass('fa-times');
              $('#blackbox').css(_5ch.css.display_all()).show();
            }
            e.stopPropagation();
            e.preventDefault();
          });
          return this;
        },
        go_top: function () {
          $('#go_top').bind('click', function () {
            $('html,body').animate({
              scrollTop: 0
            }, 'normal');
            $('#blackbox').trigger('click');
          });
          return this;
        },
        go_bottom: function () {
          $('#go_bottom').bind('click', function () {
            $('html,body').animate({
              scrollTop: $(document).height()
            }, 'normal');
            $('#blackbox').trigger('click');
          });
          return this;
        },
        goForm: function () {
          $('.js-go_writeform').bind('click', function () {
            _5ch.scroll.to($('#js-forms'));
          });
          return this;
        },
        click_blackbox: function () {
          $('#blackbox').bind('click', function (e) {
            $('#blackbox').hide();
            $('#search_input_text').blur();
            _5ch.navButton.hide();
          });
          return this;
        }
      },
      getSubDomain: function () {
        return $('#board_domain').val().split('.')[0];
      },
      getThumbnailURLByDatNumber: function (dat_number) {
        if (!_5ch.Subback.downloaded.data.thumbnails) return false;
        var url = _5ch.Subback.downloaded.data.thumbnails[dat_number];
        if (!url) return false;
        url = url.replace(/thumb.\.itest5ch\.net/, 'itest.bbspink.com');
        if (url.match(/^http:\/\/itest\.bbspink\.com\/app\/show_thumbnail/)) {
          return url;
        } else {
          if (url.match(/^http:\/\/i\.ytimg\.com\/vi\//)) {
            return url;
          } else {
            return _5ch.text.genThumbnailUrl(_5ch.Subback.downloaded.data.thumbnails[dat_number]);
          }
        }
      },
      now_loading: function () {
        var window_height = $(window).height();
        $('#subback').find('.loading').css({
          'top': ((window_height / 2) + $('#header').height()) + 'px',
          'height': window_height + 'px'
        });
        return this;
      },
      show: function (fn) {
        $('.js-pcurl').each(function (k, v) {
          var $v = $(v);
          $v.attr('href', location.protocol + '//' + $('#board_domain').val() + '/' + _5ch.request.board + $v.attr('href'));
        });
        _5ch.ronin.checkStatus();
        $.when(this.download({
          board: _5ch.request.board
        }), _5ch.Ad.download({
          url: _5ch.request.board
        })).done(function () {
          $('#subback').empty();
          _5ch.Subback.render('#subback-tmpl', '#subback');
          if (fn) fn();
        }).fail(function () {
          $('#subback').html(_5ch.error_messages.subback.disconnect);
        });
      },
      create_or_update: function () {
        var db = _5ch.db.load();
        if (!db) return false;
        var name = $('#header_title').text().trim();
        var select_subback = _5ch.db.select_first(db, "subbacks", {
          board: _5ch.request.board
        });
        if ($.present(select_subback)) {
          if ((_5ch.time.unix() - select_subback.updated_at) >= 10) {
            var update_fn = function (row) {
              row.name = name;
              row.body = "";
              row.access_count += 1;
              row.updated_at = _5ch.time.unix();
              return row;
            };
          } else {
            var update_fn = function (row) {
              row.name = name;
              row.body = "";
              row.access_count += 1;
              return row;
            };
          }
          _5ch.db.update(db, "subbacks", {
            board: _5ch.request.board
          }, update_fn);
        } else {
          _5ch.db.create(db, "subbacks", {
            board: _5ch.request.board,
            name: name,
            body: "",
            access_count: 1,
            created_at: _5ch.time.unix(),
            updated_at: _5ch.time.unix()
          });
        }
      },
      getBoardNameByURL: function () {
        return window.location.href.replace('bbspink/', '').match(/https?:\/\/*.+\/([0-9A-Za-z]+)/).pop();
      },
      genURL: function (params) {
        var url = _5ch.env.location_origin;
        url += '/subbacks/' + params['board'] + '.json';
        return url;
      },
      download: function (params) {
        var url = "";
        url = this.genURL(params);
        return $.ajax({
          type: 'GET',
          url: url,
          async: true,
        }).success(function (response) {
          $('#subback_loading').html('(｀・ω・´)描画しています');
          if ($.present(response)) {
            _5ch.Subback.downloaded.at = _5ch.time.unix();
            _5ch.Subback.downloaded.data = _5ch.JSON.check(response) ? response : JSON.parse(response);
            _5ch.Subback.orderBy();
          } else {
            $('#subback_loading').html(_5ch.error_messages.subback.disconnect);
          }
        });
      },
      orderBy: function () {
        var orderby = location.search.match(/^\?orderby\=(new|ikioi|mita|midoku)$/);
        orderby = orderby ? orderby[1] : 'normal';
        if (orderby != 'normal') _5ch.Subback.downloaded.data.threads = _5ch.Subback.sortBy(orderby);
        $('#js-orderby .orderby-' + orderby).addClass('selected highlight');
        $('#js-orderby .orderby-' + orderby).html('<i class="fa fa-check fa-padding-right" />' + $('#js-orderby .orderby-' + orderby).html());
      },
      render: function (template_selector, selector, fn, data) {
        if (!data) data = this.downloaded.data.threads;
        var first = $(selector + ' li:not(.ad)').length;
        var last = 50;
        var template = _5ch.template.ejs.compile(template_selector);
        var threads = $.grep(data, function (v, k) {
          return (k >= first && k < (first + last));
        });
        $(selector).append(template({
          threads: threads
        }));
        if (threads.length == last) $('#subback').append('<li class="center"><a href="#" id="more_subback">もっとスレッドを見る<i class="fa fa-angle-down"></i></a></li>');
        if (threads.length == last || data.length === threads.length) {
          if (_5ch.env.site_type === 'bbspink') {
            var scripttag = '<script src="//i.socdm.com/sdk/js/adg-script-loader.js?id=46151&targetID=adg_46151&displayid=1&adType=RECT&async=false&tagver=2.0.0"></script>';
          } else {
            if (_5ch.env.controller == 'subback') {
              var scripttag = '<script src="//i2ad.jp/customer/gmn/itest.php?id=6"></script>';
            }
            if (_5ch.env.controller == 'thread') {
              var scripttag = '<script src="//i2ad.jp/customer/gmn/itest.php?id=7"></script>';
            }
          }
          $('#subback').append('<li id="instant_ad" class="center"></li>');
          var targetdom = document.getElementById('instant_ad');
          _5ch.dom.addIframe(targetdom, scripttag, 300, 250);
        }
        _5ch.Ad.network();
        if (fn) fn();
      },
      sortBy: function (target) {
        var extension = false;
        switch (target) {
        case ('res'):
          target = 1;
          break;
        case ('new'):
          target = 3;
          extension = true;
          break;
        case ('midoku'):
          target = 6;
          extension = true;
          break;
        case ('mita'):
          target = 7;
          extension = true;
          break;
        case ('ikioi'):
          target = 8;
          extension = true;
          break;
        }
        return _5ch.Subback.arsort(_5ch.Subback.downloaded.data.threads, target, extension);
      },
      arsort: function (hash, key, extension) {
        if (extension) {
          _hash = [];
          try {
            var cookies = $.cookie();
          } catch (e) {
            var cookies = {};
            $(document.cookie.split('; ')).each(function (k, v) {
              var vv = v.split('=');
              cookies[vv[0]] = vv[1];
            });
          }
          $.each(hash, function (k, v) {
            var c = cookies[v[3].replace('/', '_')];
            v[6] = c ? (v[1] - c) : 0;
            v[7] = c ? c : 0;
            v[8] = Math.ceil(v[1] / ((parseInt((new Date) / 1000) - v[3].split('/').pop()) / 86400));
            _hash.push(v);
          });
        }
        return hash.sort(function (a, b) {
          return (a[key] < b[key]) ? 1 : -1;
        });
      }
    },
    Thread: {
      infeed_count: 0,
      addEvent: {
        changeFontSize: function () {
          var get = function (key) {
            return (_5ch.env.browsing_mode === 'private') ? $.cookie(key) : _5ch.db.get(key);
          };
          var set = function (key, value) {
            return (_5ch.env.browsing_mode === 'private') ? $.cookie(key, value) : _5ch.db.set(key, value);
          };
          var fontsizes = {
            'small': '小',
            'normal': '中',
            'large': '大'
          };
          var $nav = $('#js-fontsize').find('.nav_icon');
          var fontsize = get('font-size');
          if (!fontsizes[fontsize]) {
            fontsize = 'normal';
            set('font-size', fontsize);
          }
          $nav.addClass('forced_fontsize-' + fontsize).text(fontsizes[fontsize]);
          $('.threadview').addClass('threadview-' + fontsize);
          $('#js-fontsize').on('click', function () {
            var $nav = $(this).find('.nav_icon');
            var fontsize = get('font-size');
            $nav.removeClass('forced_fontsize-' + fontsize);
            var nextsize = '';
            switch (fontsize) {
            case 'small':
              nextsize = 'normal';
              break;
            case 'normal':
              nextsize = 'large';
              break;
            case 'large':
              nextsize = 'small';
              break;
            }
            $nav.addClass('forced_fontsize-' + nextsize).text(fontsizes[nextsize]);
            set('font-size', nextsize);
            $('.threadview').removeClass('threadview-small').removeClass('threadview-large');
            $('.threadview').addClass('threadview-' + nextsize);
          });
          return this;
        },
        searchResponse: function () {
          var calcResponses = function (word) {
            var responses = [];
            if (word) {
              var regex = new RegExp(word, 'g');
              _5ch.Thread.getCommentsAll().forEach(function (v, k) {
                var res = v.slice();
                var m = res[6].indexOf(word);
                if (m === -1) return true;
                res[6] = res[6].replace(regex, '<span class="highlight">' + word + '</span>');
                responses.push(res);
              });
            }
            return responses;
          };
          var search = function () {
            $('#header_title').addClass('hidden');
            $('#header_search_form').removeClass('hidden');
            $('#header').removeClass('header-fixed').addClass('header-absolute').css({
              top: $(window).scrollTop() + 'px'
            });
            $('#header').css({
              top: $(window).scrollTop() + 'px'
            });
            setTimeout(function () {
              $('#header').css({
                top: $(window).scrollTop() + 'px'
              });
            }, 300);
            var word = $('#header_search').val();
            $('#header_search').focus();
            if (word) {
              var responses = calcResponses(word);
              if (responses.length > 0) {
                _5ch.Thread.showCardView(null, responses, 'レス検索' + '(' + responses.length + '件)');
                _5ch.eventTrack('カードビュー表示', 'クリック', 'レス検索');
              } else {
                alert(word + 'が含まれたレスは見つかりませんでした');
              }
              $('#header').addClass('header-fixed').removeClass('header-absolute');
              $('#header').css({
                top: '0'
              });
            }
          };
          $('#header_search_form').submit(function (e) {
            e.preventDefault();
            search();
            $('#header_search').blur();
          });
          $('#header_search').blur(function () {
            $('#header_title').removeClass('hidden');
            $('#header_search_form').addClass('hidden');
            setTimeout(function () {
              $('#header_search').val('');
              $('#header').addClass('header-fixed').removeClass('header-absolute');
              $('#header').css({
                top: '0'
              });
            }, 300);
          });
          $('.js-res-search').click(function () {
            search();
          });
          return this;
        },
        showReply: function () {
          $('#thread').on('click', '.reply_to', function () {
            var $this = $(this);
            var $icon = $this.find('.fa-reply');
            if ($icon.length > 0) {
              $this.html('<i class="fa fa-times"/>閉じる');
              var $li = $this.closest('.threadview_response');
              var num = $li.prop('id').replace('res_', '');
              $('#res_' + num).after($('#replyform').removeClass('hidden').hide().slideDown());
              $('#js-replyform').val('>>' + num + '\n');
            } else {
              $this.html('<i class="fa fa-reply"/>返信');
              $('#replyform').slideUp();
              $this.find('.fa-times').removeClass('fa-times').addClass('fa-reply');
            }
            return false;
          });
          return this;
        },
        showReplyLink: function () {
          $('#thread').on('touchend', '.threadview_response', function (e) {
            var $this = $(this);
            if ($this.find('.reply_to').length === 0) {
              $('.reply_to').remove();
              var $d = $this.find('.threadview_response_detail');
              var $e = $('<a>').addClass('nolink reply_to').prop('href', '#').html('<i class="fa fa-reply"/>返信');
              $d.prepend($e);
            }
          });
          return this;
        },
        loadSubbackOnBottom: function () {
          var loaded = false;
          var st = $(document).scrollTop();
          var dh = $(document).height();
          var wh = $(window).height();
          var check_scroll_bottom = setInterval(function () {
            if (loaded) return false;
            if ($(".threadview_response").length == 0) return true;
            st = $(document).scrollTop();
            if ((st + (wh * 2)) >= dh) {
              loaded = true;
              clearInterval(check_scroll_bottom);
              $.when(_5ch.Subback.download({
                board: _5ch.request.board
              })).done(function () {
                $('#subback').empty();
                _5ch.Subback.render('#subback-tmpl', '#subback');
              });
            }
          }, 300);
          return this;
        },
        moveNewResponse: function () {
          $('#move_new_response').on('click', function () {
            var response = $(this).data('response');
            if (!response) return false;
            var res = document.querySelector('#res_' + response);
            if (res) _5ch.scroll.to($(res).next());
            $('#float_move_new_response').fadeOut();
            return false;
          });
          return this;
        },
        changeCharset: function () {
          $('.postform').submit(function () {
            var message = $(this).find('[name="MESSAGE"]').val();
            if (!message) return true;
            if (message.replace(/\n/, '').match(/^>>[\d]{1,4}$/)) {
              if (window.confirm('書き込みをしても宜しいですか?') === false) {
                return false;
              }
            }
            document.charset = 'Shift-JIS';
            if ($.cookie('track-write-button') != _5ch.request.board) {
              $.cookie('track-write-button', _5ch.request.board, {
                expires: 1,
                path: '/'
              });
              _5ch.eventTrack('書込ボタン', 'クリック', _5ch.request.board);
            }
          });
          return this;
        },
        floatNav: function () {
          var floatNavToggle = $('.float-nav .toggle');
          floatNavToggle.on('click', function (e) {
            var $nav = $(this).closest('nav');
            var $table = $nav.find('table');
            $nav.toggleClass('closed');
            $table.toggleClass('hidden');
            if ($nav.hasClass('closed')) {
              floatNavToggle.find('.fa.fa-times').removeClass('fa-times').addClass('fa-bars');
              $('#blackbox').hide();
            } else {
              floatNavToggle.find('.fa.fa-bars').removeClass('fa-bars').addClass('fa-times');
              $('#blackbox').css(_5ch.css.display_all()).show();
            }
            e.stopPropagation();
            e.preventDefault();
          });
          return this;
        },
        jumpPage: function () {
          $('#main').on('click', '.threadview_response_body_link', function (e) {
            if (this.href === 'http://premium.5ch.net/') {
              window.open('http://premium.5ch.net/?id=itest');
              return false;
            }
            if (this.href.match(/(thench\.net|bbspink\.com|2ch\.net|5ch\.net|machi\.to)/)) return true;
            if ($(this).hasClass('nolink')) return;
            var jump_url = [location.protocol, '//', location.hostname].join('');
            if (_5ch.env.site_type == '5ch') {
              jump_url += '/jump/to';
            } else {
              jump_url += (_5ch.env.board_type == '5ch') ? '/jump/to' : '/jump/to/bbspink';
            }
            jump_url += '?url=' + encodeURI(this.href);
            _5ch.eventTrack('外部URL', 'クリック', this.href);
            window.open(jump_url);
            return false;
          });
          return this;
        },
        popup_res: function () {
          $('#main').on("click", ".threadview_response_body_link-anchor", function (e) {
            var $currentTarget = $(e.currentTarget);
            var num = $currentTarget.text().replace('>>', '');
            var html = _5ch.Thread.getCommentHtmlById(num);
            var $popup_clone = $('#popup').clone(true);
            var $popup_body = $popup_clone.find('.popup_body');
            $popup_clone.removeAttr('id').addClass('popup_clone');
            $popup_body.html(html + '<div class="popup_closebox"><i class="fa fa-close popup_closebox_button"></i></div>');
            $('#main').append($popup_clone);
            if ($popup_body.height() > 200) $popup_body.css({
              height: '200px'
            });
            $popup_clone.css({
              'top': ($currentTarget.offset().top) + 'px'
            }).removeClass('popup-hidden');
            var $imgs = $popup_clone.find('.lazy-image');
            $imgs.each(function (k, v) {
              var $img = $(v);
              $img.prop('src', $img.data('original'));
            });
          });
          return this;
        },
        close_comment_window: function () {
          $('#main').on('click', '.popup_closebox', function (e) {
            $(e.currentTarget).closest('.popup_clone').remove();
          });
          $('#thread').bind('click', function () {
            $('.popup_clone').remove();
          });
          return this;
        },
        toggleImageViewTools: function () {
          $('#main').on("click", ".imageview_sourcebox", function (e) {
            $('.js-toggle-tools').toggleClass('hidden');
            $('.imageview_status').toggleClass('hidden');
            $('.imageview_rect').toggleClass('imageview_rect-bottom0');
          });
          return this;
        },
        click_closebox: function () {
          $('#close_thumbnail').bind('click', function () {
            $('#blackbox').trigger('click');
            $('#imageview_image').prop('src', '/assets/img/loading.gif');
          });
          return this;
        },
        more_res: function () {
          $('#reload_thread').bind('click', function () {
            $('#reload_thread').html('読み込んでいます、少々お待ち下さい<i class="fa fa-spinner fa-spin"></i> ');
            setTimeout(function () {
              $.when(_5ch.Thread.download({}, true)).done(function (downloaded) {
                var since = _5ch.Thread.downloaded.data.total_count + 1;
                try {
                  downloaded = _5ch.JSON.check(downloaded) ? downloaded : JSON.parse(downloaded);
                } catch (e) {
                  $('#reload_thread').html('最新コメを読み込む<i class="fa fa-angle-down"></i>');
                  return false;
                }
                _5ch.Thread.update_downloaded_cache(since, downloaded);
                _5ch.Thread.render('#thread-tmpl', '#thread', function () {
                  _5ch.Ad.network();
                });
                $('#reload_thread').html('最新コメを読み込む<i class="fa fa-angle-down"></i>');
              }).fail(function () {
                console.log('download fail');
              });
            }, 300);
          });
          return this;
        },
        go_top: function () {
          $('#go_top').bind('click', function () {
            _5ch.scroll.top();
            $('#blackbox').trigger('click');
          });
          return this;
        },
        go_bottom: function () {
          $('.go_bottom').bind('click', function () {
            _5ch.scroll.to($('#reload_thread'));
            $('#blackbox').trigger('click');
          });
          return this;
        },
        popup_thumbnail: function () {
          var images = [];
          var getImageUrls = function () {
            var urls = $.map(_5ch.Thread.getCommentsAll(), function (k, v) {
              return k[6].match(_5ch.regexp.urls.all);
            });
            images = $.grep(urls, function (url, k) {
              return (url.match(_5ch.regexp.urls.image));
            });
            images = $.map(images, function (url, k) {
              return _5ch.Thread.convert.fix_url(url);
            });
            images = images.filter(function (v, k, s) {
              return s.indexOf(v) === k;
            });
          };
          var setPagingImageUrl = function (current_image_url) {
            if (images.length === 0) getImageUrls();
            var pos = images.indexOf(current_image_url);
            if (pos === 0) {
              $('.imageview_prev').data('paging-image-url', images[images.length - 1]);
              $('.imageview_next').data('paging-image-url', images[pos + 1]);
            } else if (images.length === (pos + 1)) {
              $('.imageview_prev').data('paging-image-url', images[pos - 1]);
              $('.imageview_next').data('paging-image-url', images[0]);
            } else {
              $('.imageview_prev').data('paging-image-url', images[pos - 1]);
              $('.imageview_next').data('paging-image-url', images[pos + 1]);
            }
            $('.imageview_status').text((pos + 1) + '/' + images.length);
          };
          $('#main').on("click", "a.threadview_response_body_imagelink", function (e) {
            e.preventDefault();
            $('head meta[name="referrer"]').remove();
            var meta = document.createElement('meta');
            var $meta = $(meta).attr('name', 'referrer').attr('content', 'no-referrer');
            $('head').append($meta);
            $('#imageview').removeClass('imageview-hidden');
            var image_url = e.currentTarget.dataset.url;
            var ad_url = "";
            if (itest5ch.env.site_type == '5ch') {
              ad_url = '<script src="//i2ad.jp/customer/gmn/itest.php?id=4"></script>';
            } else {
              ad_url = '<script src="//i.socdm.com/sdk/js/adg-script-loader.js?id=47978&targetID=adg_47978&displayid=2&adType=RECT&async=false&tagver=2.0.0"></script>';
            }
            $('#js-imageview-ad').empty();
            _5ch.dom.addIframe(document.getElementById('js-imageview-ad'), ad_url, 300, 250);
            $('#imageview_image').prop('src', image_url);
            $('#open_thumbnail_link').prop('href', image_url);
            $('.imageview_url').text(image_url);
            $('#blackbox').css(_5ch.css.display_all()).show();
            setPagingImageUrl(image_url);
          });
          $('.imageview_prev, .imageview_next').bind('click', function (e) {
            var image_url = $(this).data('pagingImageUrl');
            if (!image_url) return false;
            $('#imageview_image').prop('src', '/assets/img/loading.gif');
            $('#open_thumbnail_link').prop('href', image_url);
            $('.imageview_url').text(image_url);
            setTimeout(function () {
              $('#imageview_image').prop('src', image_url);
            }, 100);
            setPagingImageUrl(image_url);
          });
          return this;
        },
        click_blackbox: function () {
          $('#blackbox').bind('click', function () {
            $('#blackbox').hide();
            $('#imageview').addClass('imageview-hidden');
            $('head meta[name="referrer"]').remove();
            var meta = document.createElement('meta');
            var $meta = $(meta).attr('name', 'referrer').attr('content', 'always');
            $('head').append($meta);
            setTimeout(function () {
              $('#header').removeClass('position_fixed');
            }, 300);
            _5ch.navButton.hide();
          });
          return this;
        },
        show_response: function () {
          $('#thread').on('click', '.js-response', function (e) {
            if ($('.popup_clone').length > 0) return;
            var own = $(this).closest('.threadview_response').html();
            var own_id = this.id.replace('responser_id_', '');
            var comments = _5ch.Thread.getCommentsAll();
            var responses = [];
            comments.forEach(function (v, k) {
              var comment = v[6];
              var m = comment.match(_5ch.regexp.response_anchor);
              if (!m) return true;
              if (m.indexOf('&gt;&gt;' + own_id) > -1) {
                responses.push(v);
              }
            });
            if (responses.length > 0) {
              _5ch.Thread.showCardView(own, responses);
              _5ch.eventTrack('カードビュー表示', 'クリック', '任意宛てのレス一覧');
            }
          });
          return this;
        },
        showCommentsByID: function () {
          $(document).on('click', '.res_id', function () {
            var matches = this.text.match(/ID:(.+)(\(.{3,}\))/);
            if (!matches) return false;
            if (matches.length != 3) return false;
            own_id = matches[1];
            var comments = _5ch.Thread.getCommentsAll();
            var responses = [];
            comments.forEach(function (v, k) {
              var id = v[4];
              if (own_id != id) return true;
              responses.push(v);
            });
            if (responses.length > 0) {
              _5ch.Thread.showCardView(null, responses, 'ID:' + own_id + '(' + responses.length + ')');
              _5ch.eventTrack('カードビュー表示', 'クリック', 'ID抽出');
            }
            return false;
          });
          return this;
        }
      },
      showCardView: function (own, responses, title) {
        if (!title) title = '返信';
        var template = _5ch.template.ejs.compile('#thread-response-tmpl');
        var $responseview = $('#responseview');
        var $card_contents = $('#card_contents');
        var treeHTML = "";
        if (own != null) {
          treeHTML = "<div class='card_content first js-cardview_ad-320x180 center'></div>";
          treeHTML += "<div class='card_content large'>" + own + "</div>";
        }
        $('#card_title').text(title);
        $responseview.removeClass('hidden');
        var class_names = '';
        responses.forEach(function (v, k) {
          class_names = (own == null && k == 0) ? 'card_content first' : 'card_content';
          if (k === 0 || (own == null && k === 1)) {
            var ad_class_name = (k === 0 && _5ch.env.site_type === '5ch' && own == null) ? ' js-cardview_ad-320x180 ' : ' js-cardview_ad-320x50 ';
            treeHTML += "<div class='" + class_names + ad_class_name + " center'></div>";
          }
          if ((k > 0 && (responses.length != (k + 1)) && (k % 5) === 0)) treeHTML += "<div class='" + class_names + " js-cardview_ad-320x50 center'></div>";
          class_names = 'card_content';
          treeHTML += "<div class='" + class_names + "'>" + template({
            comment: v
          }) + "</div>";
        });
        if (itest5ch.env.site_type === 'bbspink') {
          treeHTML += "<div class='card_content'>" + _5ch.Ad.Thread.to_html(_5ch.Ad.random()) + "</div>";
        } else {
          treeHTML += "<div id='js-cardview_ad' class='card_content center'></div>";
        }
        $card_contents.html(treeHTML);
        $card_contents.find('.js-response').remove();
        $card_contents.find('.reply_to').remove();
        $card_contents.removeClass('shadowclear');
        $("#card_contents .lazy-image").each(function (k, v) {
          v.src = v.dataset.original;
        });
        if (itest5ch.env.site_type === 'bbspink') {
          _5ch.Ad.network();
          var scripttag = "<script type='text/javascript' src='//js.passaro-de-fogo.biz/t/192/469/a1192469.js'></script>";
          $('.js-cardview_ad-320x50').each(function (k, v) {
            _5ch.dom.addIframe(v, scripttag, 320, 50);
          });
        } else {
          var scripttag = '<script src="//i2ad.jp/customer/gmn/itest.php?id=1"></script>';
          var targetdom = document.getElementById('js-cardview_ad');
          if (targetdom) _5ch.dom.addIframe(targetdom, scripttag, 300, 250);
          var scripttag = "<script type='text/javascript' src='//i2ad.jp/i/iVCCp0kwKCl4/sync'></script><script type='text/javascript'>_iads.outAd(160);</script>";
          $('.js-cardview_ad-320x50').each(function (k, v) {
            _5ch.dom.addIframe(v, scripttag, 320, 50);
          });
          $('.js-cardview_ad-320x180').each(function (k, v) {
            _5ch.dom.addIframe(v, _5ch.Ad.tags.thread.cardview_320x180, 320, 180);
          });
        }
        setTimeout(function () {
          $responseview.removeClass('traslated');
        }, 10);
        $('#js-cardback').on('click', function (e) {
          $responseview.addClass('traslated shadowclear');
          setTimeout(function () {
            $responseview.addClass('hidden');
            $card_contents.html('');
          }, 500);
        });
      },
      max_retry_count: 5,
      location_pathname: !!location.pathname.match(/test\/read\.cgi/) ? location.pathname.match(/(\/test\/read\.cgi\/[0-9a-zA-Z]+\/[0-9]+\/?(l?([0-9]+)?n?\-?([0-9]+)?)n?)/)[0] : '',
      showNewResponseButton: function () {
        var response = $('#move_new_response').data('response');
        if (!_5ch.Thread.get_url_suffix() && response > 0) {
          $('#float_move_new_response').removeClass('hidden');
        }
        setTimeout(function () {
          $('#float_move_new_response').fadeOut();
        }, 10000);
      },
      getCommentByResNum: function (res_num) {
        if (!res_num) return false;
        var comment = null;
        _5ch.Thread.getCommentsAll().forEach(function (k, v) {
          if (v[0] == res_num) comment = v;
        });
        return comment;
      },
      loadSproutAd: function () {
        var scripttag = null;
        if (_5ch.env.site_type === '5ch') {
          scripttag = '<script type="text/javascript">' + 'var adstir_vars = {' + '  ver: "4.0",' + '  app_id: "MEDIA-8ccac345",' + '  ad_spot: 1,' + '  center: false' + '};' + '</script>' + '<script type="text/javascript" src="https://js.ad-stir.com/js/adstir.js"></script>';
        }
        if (itest5ch.env.site_type === 'bbspink') {
          scripttag = '<script type="text/javascript" src="https://js.sprout-ad.com/t/004/830/a1004830.js"></script>';
        }
        if (scripttag) {
          var targetdom = document.getElementsByClassName('sproutad_frame')[0];
          _5ch.dom.addIframe(targetdom, scripttag, 300, 250, "", false);
        }
        $('.sproutad_frame').removeClass('hidden');
      },
      getCommentHtmlById: function (id) {
        var html = $('#res_' + id).html();
        if (!html) {
          var found = false;
          var comment = _5ch.Thread.getCommentsAll()[(id - 1)];
          if (comment) {
            if (comment[0] == id) {
              var template = _5ch.template.ejs.compile($('#thread-response-tmpl'));
              html = template({
                comment: comment,
                helper: _5ch.template.thread.ejs
              });
            }
          }
        }
        return html;
      },
      saveThreadData: function () {
        if (_5ch.env.browsing_mode == 'normal') {
          _5ch.db.init();
          var db = _5ch.db.load();
          db.insertOrUpdate("threads", {
            board: _5ch.request.board,
            dat: _5ch.request.dat_number
          }, {
            board: _5ch.request.board,
            dat: _5ch.request.dat_number,
            title: _5ch.Thread.downloaded.data.thread[5],
            response_count: _5ch.Thread.getLastId(),
            subdomain: _5ch.Thread.getSubDomain(),
            created_at: _5ch.time.unix(),
            updated_at: _5ch.time.unix(),
          });
          db.commit();
        }
      },
      update_downloaded_cache: function (since, downloaded) {
        since--;
        _5ch.Thread.downloaded.data.total_count = downloaded.total_count;
        _5ch.Thread.downloaded.data.comments = downloaded.comments;
        $.cookie(_5ch.request.board + '_' + _5ch.request.dat_number, downloaded.total_count, {
          expires: 7,
          path: '/'
        });
        _5ch.Thread.saveThreadData();
      },
      getDomainName: function () {
        var hostex = location.hostname.split('.');
        return hostex[hostex.length - 2] + '.' + hostex[hostex.length - 1];
      },
      getSubDomainNameFromURL: function () {
        if (location.pathname.indexOf('test/read.cgi') === -1) return false;
        return window.location.href.match(/\/([a-zA-Z0-9\-]+)?\/?test\/read.cgi\/([0-9A-Za-z]+)\/([0-9]+)\/?(-?l?[0-9]+n?-?)?/)[1];
      },
      getSubDomain: function () {
        return !!_5ch.Thread.getSubDomainNameFromURL() ? (_5ch.Thread.getSubDomainNameFromURL() + '.' + _5ch.Thread.getDomainName()) : $('#board_domain').val();
      },
      setDatNumber: function () {
        $('.js-dat_number').val(_5ch.request.dat_number);
      },
      updateBackSubbackLinks: function () {
        var orderby = document.referrer.match(/subback\/[\d\w]+\?orderby=(\w+)/);
        if (orderby) $('.js-backsubback').attr('href', $('.js-backsubback').attr('href') + '?orderby=' + orderby[1]);
      },
      initialize: function () {
        _5ch.Thread.url.parse();
        _5ch.db.init();
        _5ch.setting.font.load();
        _5ch.env.controller = 'thread';
        var origin = [location.protocol, '//', location.hostname].join('');
        var url = ['/test/read.cgi/', _5ch.request.board, '/', _5ch.request.dat_number].join('');
        $('.js-inurl').each(function (k, v) {
          var $v = $(v);
          $v.attr('href', origin + url + $v.attr('href'));
        });
        var subdomain = _5ch.Thread.getSubDomain();
        $('.js-pcurl').each(function (k, v) {
          $(v).attr('href', location.protocol + '//' + subdomain + _5ch.Thread.location_pathname + $(v).attr('href'));
        });
        _5ch.addEvent.toggle_header('#thread');
        if (_5ch.request.board == 'avideo' && _5ch.request.dat_number == '1257933389') {
          return _5ch.Thread.transferPCView();
        }
        _5ch.ronin.checkStatus();
        _5ch.Thread.setDatNumber();
        _5ch.Thread.updateBackSubbackLinks();
        _5ch.Thread.setCanonicalURL();
      },
      setCanonicalURL: function () {
        var link = document.createElement('link');
        link.setAttribute('rel', 'canonical');
        link.setAttribute('href', 'https://' + location.host + '/' + _5ch.Thread.getSubDomain().split('.')[0] + '/test/read.cgi/' + _5ch.request.board + '/' + _5ch.request.dat_number);
        document.head.appendChild(link);
      },
      transferPCView: function () {
        window.location = location.protocol + '//' + _5ch.Thread.getSubDomain() + _5ch.Thread.location_pathname + '?v=pc';
        return true;
      },
      get_url_suffix: function () {
        var suffix = _5ch.Thread.location_pathname.replace('/bbspink', '').split('/')[5];
        return !!suffix ? suffix : false;
      },
      downloaded_content_present: function (downloaded) {
        if ($.present(downloaded)) {
          if ($.present(downloaded.comments)) {
            return true;
          }
        }
        return false;
      },
      fixTitleHeight: function () {
        var $header_title = $('#header_title');
        var class_name = ($header_title.text().length > 15) ? 'header_cell_title-fontsmall' : 'header_cell_title-fontnormal';
        $header_title.addClass(class_name);
        return this;
      },
      retryDownlonadAndRender: function (fn, retry) {
        retry += 1;
        _5ch.Thread.download_and_render(fn, retry);
      },
      download_and_render: function (fn, retry) {
        if (!retry) retry = 0;
        $.when(_5ch.Thread.download({}, true), _5ch.Ad.download({
          url: _5ch.request.board
        })).done(function (thread, ad) {
          if ($.present(thread[0])) {
            _5ch.Thread.initialize_render();
            if (fn) fn();
          } else {
            if (retry >= _5ch.Thread.max_retry_count) return _5ch.Thread.transferPCView();
            _5ch.Thread.retryDownlonadAndRender(fn, retry);
          }
        }).fail(function () {
          if (retry >= _5ch.Thread.max_retry_count) return _5ch.Thread.transferPCView();
          _5ch.Thread.retryDownlonadAndRender(fn, retry);
        });
      },
      check_more_res: function () {
        setTimeout(function () {
          $('#reload_thread').trigger('click');
        }, 3000);
      },
      showThreadStatus: function (status) {
        if (status == '-2') return $('#thread_status_bar').text("■ レス数が1000に到達しました");
        if (status == '-1') return $('#thread_status_bar').text("■ レス数が1000近いです もうすぐ書けなくなります");
        if (status == '2') return $('#thread_status_bar').text("■ このスレッドはdat落ちしています");
        if (status == '3') return $('#thread_status_bar').text("■ このスレッドは過去ログ倉庫に格納されています");
        $('#thread_status_bar').remove();
      },
      initialize_render: function () {
        if (this.downloaded.data == "") return false;
        var title = this.getDataAll().thread[5];
        this.render('#thread-tmpl', '#thread');
        $('#reload_thread').removeClass('hidden');
        $('#header_title, #title').html(title);
        $('title').html(title + ' | ' + $('title').text());
        this.fixTitleHeight();
        $('.js-render-after').removeClass('hidden');
        this.showThreadStatus(this.getDataAll().thread[6]);
        var self = this;
        self.socialbutton();
        setTimeout(function () {
          self.showBottomAd();
        }, 5000);
        return true;
      },
      showBottomAd: function () {
        switch (_5ch.env.site_type) {
        case '5ch':
          var ad = '<script type="text/javascript">' + 'var adstir_vars = {' + '  ver: "4.0",' + '  app_id: "MEDIA-8ccac345",' + '  ad_spot: 67,' + '  center: false' + '};' + '</script>' + '<script type="text/javascript" src="https://js.ad-stir.com/js/adstir.js"></script>';
          break;
        case 'bbspink':
          var ad = '<script src="//i.socdm.com/sdk/js/adg-script-loader.js?id=46150&targetID=adg_46150&displayid=1&adType=RECT&async=false&tagver=2.0.0"></script>';
          break;
        }
        var targetdom = document.getElementById('js-bottom-ad-300x250');
        _5ch.dom.addIframe(targetdom, ad, 300, 250, "", false);
      },
      socialbutton: function () {
        $(document).on('click', '.socialwrap_link', function (event) {
          var href = $(this).prop("href");
          if ($(this).hasClass('socialwrap_link-twitter')) {
            $(this).prop("href", href + $.param({
              "url": location.href,
              "text": $('#title').text()
            }));
          }
          if ($(this).hasClass('socialwrap_link-line')) {
            $(this).prop("href", href + $.param({
              "url": location.href
            }));
          }
        });
      },
      id_counts: {},
      url: {
        parse: function () {
          var i = window.location.href.match(/\/([a-zA-Z0-9\-]+)?\/?test\/read.cgi\/([0-9A-Za-z]+)\/([0-9]+)\/?(-?l?[0-9]+n?-?)?/);
          _5ch.request.subdomain = i[1];
          _5ch.request.board = i[2];
          _5ch.request.dat_number = i[3];
          _5ch.request.suffix = i[4];
        }
      },
      downloaded: {
        data: "",
        at: 0
      },
      api: true,
      getDataAll: function () {
        return this.downloaded.data;
      },
      getCommentsAll: function () {
        return this.downloaded.data.comments;
      },
      didViewAll: function () {
        return (_5ch.Thread.getViewedLastId() == _5ch.Thread.getLastId());
      },
      getViewedLastId: function () {
        var id = 0;
        id = $('#thread .threadview_response:not(.res_ad)').last()[0].id;
        id = id.match(/res_([0-9]+)/)[1] - 0;
        return id;
      },
      getLastId: function () {
        var comments = _5ch.Thread.getCommentsAll();
        var last_comment = comments[comments.length - 1];
        var id = last_comment[0];
        return id;
      },
      NGThread: function (board, dat) {
        if (_5ch.request.board == board && _5ch.request.dat_number == dat) {
          window.location = _5ch.env.location_origin + '/subback/' + _5ch.request.board;
          return true;
        } else {
          return false;
        }
      },
      genDatURL: function (type) {
        var url = "";
        var subdomain = !!_5ch.request.subdomain ? _5ch.request.subdomain : $('#board_domain').val().split('.')[0];
        url = _5ch.env.location_origin + '/public/newapi/client.php%3Fsubdomain=' + subdomain + '%26board=' + _5ch.request.board + '%26dat=' + _5ch.request.dat_number + '%26rand=' + _5ch.text.rand();
        if (_5ch.db.get('use_treeview') == "true") url += "&treeview=1";
        return url;
      },
      download: function (params, cached, type) {
        if (!type) type = 'api';
        var url = this.genDatURL(type);
        if (_5ch.Thread.isFirstLoad()) {
          _5ch.Loading.show();
        }
        return $.ajax({
          type: 'GET',
          url: url,
          async: true,
          success: function (response, status, xhr) {
            _5ch.Loading.hide();
            $('#thread_loading').html('(｀・ω・´)描画しています <br /> 少々お待ち下さい <i class="fa fa-spinner fa-pulse"></i>');
            if ($.present(response) && cached == true) {
              try {
                if (_5ch.Thread.downloaded.data == "") {
                  _5ch.Thread.downloaded.at = _5ch.time.unix();
                  _5ch.Thread.downloaded.data = _5ch.JSON.check(response) ? response : JSON.parse(response);
                  if (_5ch.Thread.downloaded.data) {
                    var cookie_key = _5ch.request.board + '_' + _5ch.request.dat_number;
                    var read_point = $.cookie(cookie_key);
                    if (read_point) {
                      if ((_5ch.Thread.getLastId() - read_point) > 0) {
                        $('#move_new_response').data('response', read_point);
                      }
                    }
                    $.cookie(cookie_key, _5ch.Thread.getLastId(), {
                      expires: 7,
                      path: '/'
                    });
                    _5ch.Thread.saveThreadData();
                  } else {
                    $('#thread').html(_5ch.error_messages.thread.disconnect);
                    _5ch.Thread.transferPCView();
                  }
                }
              } catch (e) {
                $('#thread_loading').html(_5ch.error_messages.search.disconnect);
                _5ch.Thread.transferPCView();
              }
            }
          }
        });
      },
      url_suffix: function () {
        var suffix = _5ch.Thread.get_url_suffix();
        var _comments = _5ch.Thread.downloaded.data.comments;
        var data = [];
        if (suffix.match(/l\d/)) {
          suffix = parseInt(suffix.replace('l', ''))
          if (suffix > _comments.length) {
            data = _comments
          } else {
            data.push(_comments[0])
            for (var i = _comments.length - suffix; i < _comments.length; i++) {
              data.push(_comments[i])
            };
          }
          return data
        }
        if (suffix.match(/\d-\d/)) {
          suffix = suffix.split('-')
          if (suffix[0] > _comments.length) {
            data = _comments
          } else {
            data.push(_comments[0])
            for (var i = suffix[0] - 1; i < _comments.length; i++) {
              if (i >= suffix[1]) break;
              data.push(_comments[i])
            };
          }
          return data
        }
        if (suffix.match(/\dn-\d/)) {
          suffix = suffix.replace('n', '').split('-')
          if (suffix[0] > _comments.length) {
            data = _comments
          } else {
            for (var i = suffix[0] - 1; i < _comments.length; i++) {
              if (i >= suffix[1]) break;
              data.push(_comments[i])
            };
          }
          return data
        }
        if (suffix.match(/-\d/)) {
          suffix = parseInt(suffix.replace('-', ''))
          if (suffix > _comments.length) {
            data = _comments
          } else {
            for (var i = 0; i < _comments.length; i++) {
              if (i >= suffix) break;
              data.push(_comments[i])
            };
          }
          return data
        }
        if (suffix.match(/\d-/)) {
          suffix = parseInt(suffix.replace('-', ''))
          if (suffix > _comments.length) {
            data = _comments
          } else {
            data.push(_comments[0])
            for (var i = suffix - 1; i < _comments.length; i++) {
              data.push(_comments[i])
            };
          }
          return data
        }
        if (suffix.match(/\dn-?/)) {
          suffix = parseInt(suffix.replace('n', '').replace('-', ''));
          if (suffix > _comments.length) {
            data = _comments
          } else {
            for (var i = suffix - 1; i < _comments.length; i++) {
              data.push(_comments[i])
            };
          }
          return data
        }
        if (suffix == parseInt(suffix)) {
          data.push(_comments[suffix - 1])
          return data
        }
        return _comments
      },
      isFirstLoad: function () {
        return !!($('#thread_loading').length > 0);
      },
      render: function (template_selector, selector, fn) {
        var data = this.getCommentsAll();
        var template = _5ch.template.ejs.compile(template_selector);
        var first_load = true;
        if (this.get_url_suffix()) {
          data = this.url_suffix();
          $('#suffix_thread').removeClass('hidden');
          $('#suffix_thread_bottom').removeClass('hidden');
        }
        if (!this.isFirstLoad()) {
          var diff_data = [];
          var responses = $('#thread .threadview_response:not(.res_ad)');
          var since = responses[responses.length - 1].id.match(/[0-9]+/)[0];
          since -= 0;
        }
        if (this.isFirstLoad()) {
          $(selector).empty();
          var range = document.createRange();
          range.selectNodeContents(document.body);
          var flg = range.createContextualFragment(template({
            comments: data
          }));
          document.querySelector(selector).appendChild(flg);
          var new_response = $('#move_new_response').data('response');
          if (new_response > 0) $('#res_' + new_response).after('<p class="new_res">ここから新着レス</p>');
        } else {
          diff_data = this.getCommentsAll().slice(since);
          $(selector).append(template({
            comments: diff_data
          }));
        }
        if (fn) fn();
      },
      convert: {
        fix_url: function (string) {
          if (typeof string === 'string') {
            if (string.indexOf('tp://') === 0) return string.replace(/^tp:\/\//, function () {
              return 'http://' + RegExp.$1;
            });
            if (string.indexOf('ttp://') === 0) return string.replace(/^ttp:\/\//, function () {
              return 'http://' + RegExp.$1;
            });
            if (string.indexOf('tps://') === 0) return string.replace(/^tps:\/\//, function () {
              return 'https://' + RegExp.$1;
            });
            if (string.indexOf('ttps://') === 0) return string.replace(/^ttps:\/\//, function () {
              return 'https://' + RegExp.$1;
            });
            return string;
          }
        },
        BE: function (html, be_id) {
          if (!html) return false;
          if (!be_id) return html;
          if (be_id.indexOf('-') != -1) be_id = be_id.split('-')[0];
          if (html.indexOf('sssp://') === -1) return html;
          return html.replace(_5ch.regexp.sssp.BE, function (be) {
            var url = be.replace('sssp://', 'http://');
            return '<a href="https://be.5ch.net/user/' + be_id + '" target="blank">' + '<img src="/assets/img/loading.gif" class="threadview_response_body_be lazy-image" data-original="' + url + '">' + '</a>';
          });
        },
        oekaki: function (html) {
          if (!html) return false;
          if (html.indexOf('sssp://') === -1) return html;
          return html.replace(_5ch.regexp.sssp.oekaki, function () {
            var url = 'http://' + RegExp.$3;
            url = _5ch.Thread.convert.fix_url(url);
            thumbnail_url = _5ch.text.genThumbnailUrl(url);
            return '<a href="' + url + '" target="_blank" class="nolink threadview_response_body_imagelink" data-url="' + url + '"><img src="/assets/img/loading.gif" class="threadview_response_body_thumbnail lazy-image" data-original="' + thumbnail_url + '"></a>';
          });
        },
        response_anchor_to_href: function (html) {
          if (!html) return false;
          if (html.indexOf('&gt;&gt;') === -1) return html;
          return html.replace(_5ch.regexp.response_anchor, function (number) {
            number = number.replace('&gt;&gt;', '');
            return '<a class="threadview_response_body_link threadview_response_body_link-anchor anchor_' + number + ' nolink">&gt;&gt;' + number + '</a>'
          })
        },
        create_image_thumbnail_html: function (urls) {
          if (!urls) return false;
          var html = "";
          var thumbnail_url = "";
          $.each(urls, function (k, url) {
            if (url.match(_5ch.regexp.urls.image)) {
              url = _5ch.Thread.convert.fix_url(url);
              thumbnail_url = _5ch.text.genThumbnailUrl(url);
              html += '<a href="' + url + '" target="_blank" class="nolink threadview_response_body_imagelink" data-url="' + url + '"><img src="/assets/img/loading.gif" class="threadview_response_body_thumbnail lazy-image" data-original="' + thumbnail_url + '"></a>';
            }
          });
          return html;
        },
        create_video_embed_html: function (urls) {
          if (!urls) return false;
          var html = "";
          var matches = "";
          var id = "";
          $.each(urls, function (k, url) {
            var fixed_url = _5ch.Thread.convert.fix_url(url);
            matches = fixed_url.match(_5ch.regexp.urls.video.youtube);
            if (matches) {
              id = matches[1].replace('v=', '')
              html += '<a href="' + fixed_url + '" target="_blank" class="threadview_response_body_embedlink">'
              html += '<img src="http://i.ytimg.com/vi/' + id + '/default.jpg" class="threadview_response_body_thumbnail">'
              html += '</a>'
              return true;
            }
            matches = url.match(_5ch.regexp.urls.video.niconico)
            if (matches) {
              id = matches[1].replace('sm', '')
              html += '<a href="' + fixed_url + '" target="_blank" class="threadview_response_body_embedlink">'
              html += '<img src="http://tn-skr' + ((parseInt(id) % 4) + 1) + '.smilevideo.jp/smile?i=' + id + '" class="threadview_response_body_thumbnail">'
              html += '</a>'
              return true;
            }
          })
          return html
        },
        url_to_href: function (html) {
          if (!html) return false;
          return html.replace(_5ch.regexp.urls.all, function (url) {
            var classnames = "threadview_response_body_link";
            var fixed_url = _5ch.Thread.convert.fix_url(url);
            if (url.match(_5ch.regexp.urls.image)) classnames += " threadview_response_body_link-image"
            return '<a href="' + fixed_url + '" class="' + classnames + '" target="_blank">' + _5ch.text.middle_ellipsis(fixed_url) + '</a>'
          })
        }
      }
    },
    Search: {
      downloaded: {
        data: {},
        at: 0
      },
      getQuery: function (type) {
        var q = "";
        var regex = type === 'q' ? /q=(.+)/ : /board=(.+)/;
        $.each(location.search.split('&'), function (k, v) {
          var match = v.match(regex);
          if (match) q = match[1];
        });
        return q;
      },
      findPage: function (fn) {
        var q = this.getQuery('q');
        var board = this.getQuery('board');
        var params = {
          "q": decodeURIComponent(q)
        };
        this.downloadAndShow(params, "#subback", fn);
        if (board) {
          var boards = [];
          boards = $.grep($('.pure-menu-link-board'), function (v, k) {
            if (v.href == location.origin + '/subback/' + board) return true;
          });
          if (boards != []) {
            $('#js-in-board').removeClass('hidden').text(boards[0].text + '板内検索');
            params.board = board;
            this.downloadAndShow(params, "#subback-board");
          }
        }
      },
      downloadAndShow: function (params, target, fn) {
        $.when(_5ch.Search.download(params), _5ch.Ad.download()).done(function (downloaded) {
          if (0 >= _5ch.Search.downloaded.data.total_count || !_5ch.Search.downloaded.data) {
            $(target).empty().append('<li><p>検索ワードを含むスレッドは見つかりませんでした。</p></li>');
            return false;
          }
          _5ch.Search.render('#subback-tmpl', target);
          if (fn) fn();
        }).fail(function () {
          $('#subback').html(_5ch.error_messages.subback.disconnect);
        });
      },
      toOneByteString: function (string) {
        return string.replace(/[Ａ-Ｚａ-ｚ０-９]/g, function (s) {
          return String.fromCharCode(s.charCodeAt(0) - 0xFEE0);
        });
      },
      local: function (params) {
        var dfd = $.Deferred();
        var thd = [];
        var tobs = _5ch.Search.toOneByteString;
        var q = tobs(params.q.toLowerCase());
        var t = "";
        $.each(_5ch.Subback.downloaded.data.threads, function (k, v) {
          t = tobs(v[5].toLowerCase());
          if (t.indexOf(q) !== -1) {
            thd.push(v);
          }
        });
        var data = {
          total_count: thd.length,
          threads: thd
        };
        data.threads = data.threads.sort(function (a, b) {
          var a_date = a[3].split('/')[1],
            b_date = b[3].split('/')[1];
          return b_date - a_date;
        });
        _5ch.Search.downloaded.data = data;
        dfd.resolve();
        return dfd;
      },
      network: function (params) {
        var self = this;
        _5ch.Loading.show();
        return $.ajax({
          type: 'GET',
          url: 'search/v3',
          data: params,
          success: function (json) {
            _5ch.Loading.hide();
            if ($.present(json)) {
              try {
                json.result = json.result.sort(function (a, b) {
                  return b.thread_id - a.thread_id;
                });
                _5ch.Search.downloaded.data = {
                  total_count: json.result.length,
                  threads: self.toSubbackFormat(json.result),
                  thumbnails: {}
                };
                _5ch.Loading.hide();
              } catch (e) {
                _5ch.Loading.hide();
              }
            }
          },
          error: function () {
            _5ch.Loading.hide();
          }
        });
      },
      toSubbackFormat: function (json) {
        return $.map(json, function (v, k) {
          var type = v.domain.split('.')[0];
          var t = v.board_name;
          return [
            [0, v.res, v.host_name, [v.board_id, v.thread_id].join('/'), t, v.title, type]
          ];
        });
      },
      download: function (params, local) {
        if (local) {
          this.local(params);
        } else {
          var domain = (_5ch.env.site_type === '5ch') ? '5ch.net' : 'bbspink.com';
          return this.network({
            'word': params.q,
            'board': params.board,
            'domain': domain
          });
        }
      },
      render: function (template_selector, target_selector, fn) {
        var template = _5ch.template.ejs.compile(template_selector);
        $(target_selector).empty().append(template(_5ch.Search.downloaded.data));
        if (fn) fn();
      }
    },
    arsort: function (hash, key) {
      return hash.sort(function (a, b) {
        return (a[key] < b[key]) ? 1 : -1;
      });
    },
    css: {
      display_all: function () {
        return {
          'width': $('body').width() + 'px',
          'height': document.body.scrollHeight + 'px'
        }
      }
    },
    dom: {
      nolink: function () {
        $('.nolink').click(function () {
          event.preventDefault();
        });
      },
      addIframe: function (target_dom, tag, width, height, class_name, sandbox) {
        var iframe = document.createElement('iframe');
        iframe.style.width = width + "px";
        iframe.style.height = height + "px";
        iframe.style.margin = '0 auto';
        iframe.style.display = 'block';
        iframe.width = width;
        iframe.height = height;
        if (class_name) iframe.className = class_name;
        if (sandbox) iframe.sandbox = 'allow-scripts';
        if (!target_dom) return false;
        for (var i = target_dom.childNodes.length - 1; i >= 0; i--) {
          target_dom.removeChild(target_dom.childNodes[i]);
        }
        target_dom.appendChild(iframe);
        var html = "<body style='margin: 0;'>" + tag + "</body>";
        if (!iframe.contentWindow) return false;
        var iframe_document = iframe.contentWindow.document;
        iframe_document.open();
        iframe_document.write(html);
        iframe_document.close();
      },
    },
    db: {
      name: "itest5ch",
      version: 2,
      version_history: [1, 2],
      schemes: {
        scheme: ["version", "created_at", "updated_at"],
        subbacks: ["board", "name", "body", "access_count", "created_at", "updated_at"],
        threads: ["board", "title", "dat", "response_count", "subdomain", "created_at", "updated_at"]
      },
      Subback: {
        access_count_sort: function (db_objects) {
          return db_objects.sort(function (a, b) {
            return (a.access_count < b.access_count) ? 1 : -1;
          });
        }
      },
      load: function () {
        if (_5ch.env.use_localstrage() === false) return false;
        return new localStorageDB(_5ch.db.name);
      },
      init: function () {
        if (_5ch.env.use_localstrage() === false) return false;
        var db = _5ch.db.load();
        if (db.isNew()) {
          _5ch.db.createTables();
        } else {
          var scheme = db.query('scheme', {
            version: _5ch.db.version
          }).pop();
          if (!scheme) {
            _5ch.db.migrate(db.query('scheme').pop().version);
          }
        }
      },
      createTables: function () {
        if (_5ch.env.use_localstrage() === false) return false;
        var db = _5ch.db.load();
        $.each(_5ch.db.schemes, function (table_name, scheme) {
          db.createTable(table_name, scheme);
          if (table_name == 'scheme') {
            db.insert(table_name, {
              version: _5ch.db.version,
              created_at: _5ch.time.unix(),
              updated_at: _5ch.time.unix()
            });
          }
          db.commit();
        });
      },
      migrate: function (current_version) {
        if (!current_version) return false;
        _5ch.db.version_history.forEach(function (v, k) {
          if (current_version >= v) return true;
          _5ch.db.updater(v);
        });
      },
      updater: function (version) {
        var db = _5ch.db.load();
        switch (version) {
        case 2:
          db.dropTable('threads');
          db.createTable('threads', ["board", "title", "dat", "response_count", "subdomain", "created_at", "updated_at"]);
          db.update("scheme", {
            version: 1
          }, function (row) {
            row.version = version;
            row.updated_at = _5ch.time.unix();
            return row;
          });
          db.commit();
          break;
        }
      },
      select: function (db, table, where) {
        if (_5ch.env.use_localstrage() === false) return false;
        return db.query(table, where);
      },
      select_first: function (db, table, where) {
        if (_5ch.env.use_localstrage() === false) return false;
        return db.query(table, where).pop();
      },
      create: function (db, table, values) {
        db.insert(table, values);
        db.commit();
      },
      update: function (db, table, where, fn) {
        db.update(table, where, fn);
        db.commit();
      },
      get: function (name) {
        if (_5ch.env.use_localstrage() === false) return false;
        return window.localStorage.getItem(name);
      },
      set: function (name, data) {
        if (_5ch.env.use_localstrage() === false) return false;
        try {
          window.localStorage.setItem(name, data);
        } catch (e) {
          window.localStorage.removeItem(name);
          window.localStorage.setItem(name, data);
        }
        return;
      },
      remove: function (name) {
        if (_5ch.env.use_localstrage() === false) return false;
        try {
          window.localStorage.removeItem(name);
          return true;
        } catch (e) {
          return false;
        }
      }
    },
    scroll: {
      to: function (target, fn) {
        if (!target) return false;
        var $target = $(target);
        var current_pos = document.documentElement.scrollTop || document.body.scrollTop;
        var height = window.innerHeight ? window.innerHeight : $(window).height;
        var target_top = $target.offset().top;
        var position = (target_top - height + $target.outerHeight());
        var scroll_to = 0;
        var direction = (current_pos > target_top) ? 'up' : 'down';
        var base_position = (direction === 'up') ? 0 : position;
        var diff_scroll = Math.abs(base_position - current_pos);
        if (diff_scroll > 4000) {
          if (direction === 'up') window.scrollTo(0, 4000);
          if (direction === 'down') window.scrollTo(0, position - 4000);
        }
        $('html,body').animate({
          scrollTop: position
        });
        if (fn) fn();
      },
      top: function () {
        var current_pos = document.documentElement.scrollTop || document.body.scrollTop;
        if (current_pos > 4000) window.scrollTo(0, 4000);
        $('html,body').animate({
          scrollTop: 0
        });
      },
      bottom: function () {
        var current_pos = document.documentElement.scrollTop || document.body.scrollTop;
        if ($(document).height() - 4000 > current_pos) window.scrollTo(0, $(document).height() - 4000);
        $('html,body').animate({
          scrollTop: $(document).height()
        });
      },
    },
    setting: {
      font: {
        load: function () {
          var TextarFont = _5ch.db.get('TextarFont');
          if (TextarFont === null) {} else {
            var stylesheet = document.styleSheets.item(0);
            if (stylesheet.cssRules) {
              stylesheet.insertRule("@font-face { font-family: 'Textar';font-style: normal;font-weight: normal;src: url('data:font/opentype;base64," + TextarFont + "') format('woff'); }", stylesheet.cssRules.length);
              stylesheet.insertRule(".threadview_response_body-AA{font-family:'Textar';font-size:9px;line-height:1.125;white-space:nowrap;-webkit-text-size-adjust:none;}", stylesheet.cssRules.length);
            }
          }
        }
      }
    }
  }
  if (location.pathname.match(/bbspink/)) {
    _5ch.db.name = 'bbspink';
  }
  _5ch.env.set_browsing_mode();
  _5ch.env.set_site_type();
  _5ch.env.set_board_type();
  _5ch.env.checkCookies();
  window.itest5ch = _5ch;
  if (typeof define === "function" && define.amd && define.amd.itest5ch) {
    define("itest5ch", [], function () {
      return itest5ch;
    });
  }
})(window);
$.extend({
  present: function (variable) {
    return (variable != undefined && variable != "" && variable !== 0) ? true : false;
  },
  blank: function (variable) {
    return (variable != undefined && variable != "" && variable !== 0) ? false : true;
  },
  addClasses: function (arr) {
    $.each(arr, function (selector, className) {
      $(selector).addClass(className);
    });
  },
  removeClasses: function (arr) {
    $.each(arr, function (selector, className) {
      $(selector).removeClass(className);
    });
  }
})
var ___ = function () {};
(function e(t, n, r) {
  function s(o, u) {
    if (!n[o]) {
      if (!t[o]) {
        var a = typeof require == "function" && require;
        if (!u && a) return a(o, !0);
        if (i) return i(o, !0);
        var f = new Error("Cannot find module '" + o + "'");
        throw f.code = "MODULE_NOT_FOUND", f
      }
      var l = n[o] = {
        exports: {}
      };
      t[o][0].call(l.exports, function (e) {
        var n = t[o][1][e];
        return s(n ? n : e)
      }, l, l.exports, e, t, n, r)
    }
    return n[o].exports
  }
  var i = typeof require == "function" && require;
  for (var o = 0; o < r.length; o++) s(r[o]);
  return s
})({
  1: [function (require, module, exports) {
    "use strict";
    var fs = require("fs"),
      utils = require("./utils"),
      scopeOptionWarned = false,
      _VERSION_STRING = require("../package.json").version,
      _DEFAULT_DELIMITER = "%",
      _DEFAULT_LOCALS_NAME = "locals",
      _REGEX_STRING = "(<%%|<%=|<%-|<%_|<%#|<%|%>|-%>|_%>)",
      _OPTS = ["cache", "filename", "delimiter", "scope", "context", "debug", "compileDebug", "client", "_with", "rmWhitespace", "strict", "localsName"],
      _TRAILING_SEMCOL = /;\s*$/,
      _BOM = /^\uFEFF/;
    exports.cache = utils.cache;
    exports.localsName = _DEFAULT_LOCALS_NAME;
    exports.resolveInclude = function (name, filename) {
      var path = require("path"),
        dirname = path.dirname,
        extname = path.extname,
        resolve = path.resolve,
        includePath = resolve(dirname(filename), name),
        ext = extname(name);
      if (!ext) {
        includePath += ".ejs"
      }
      return includePath
    };

    function handleCache(options, template) {
      var fn, path = options.filename,
        hasTemplate = arguments.length > 1;
      if (options.cache) {
        if (!path) {
          throw new Error("cache option requires a filename")
        }
        fn = exports.cache.get(path);
        if (fn) {
          return fn
        }
        if (!hasTemplate) {
          template = fs.readFileSync(path).toString().replace(_BOM, "")
        }
      } else if (!hasTemplate) {
        if (!path) {
          throw new Error("Internal EJS error: no file name or template " + "provided")
        }
        template = fs.readFileSync(path).toString().replace(_BOM, "")
      }
      fn = exports.compile(template, options);
      if (options.cache) {
        exports.cache.set(path, fn)
      }
      return fn
    }

    function includeFile(path, options) {
      var opts = utils.shallowCopy({}, options);
      if (!opts.filename) {
        throw new Error("`include` requires the 'filename' option.")
      }
      opts.filename = exports.resolveInclude(path, opts.filename);
      return handleCache(opts)
    }

    function includeSource(path, options) {
      var opts = utils.shallowCopy({}, options),
        includePath, template;
      if (!opts.filename) {
        throw new Error("`include` requires the 'filename' option.")
      }
      includePath = exports.resolveInclude(path, opts.filename);
      template = fs.readFileSync(includePath).toString().replace(_BOM, "");
      opts.filename = includePath;
      var templ = new Template(template, opts);
      templ.generateSource();
      return templ.source
    }

    function rethrow(err, str, filename, lineno) {
      var lines = str.split("\n"),
        start = Math.max(lineno - 3, 0),
        end = Math.min(lines.length, lineno + 3);
      var context = lines.slice(start, end).map(function (line, i) {
        var curr = i + start + 1;
        return (curr == lineno ? " >> " : "    ") + curr + "| " + line
      }).join("\n");
      err.path = filename;
      err.message = (filename || "ejs") + ":" + lineno + "\n" + context + "\n\n" + err.message;
      throw err
    }

    function cpOptsInData(data, opts) {
      _OPTS.forEach(function (p) {
        if (typeof data[p] != "undefined") {
          opts[p] = data[p]
        }
      })
    }
    exports.compile = function compile(template, opts) {
      var templ;
      if (opts && opts.scope) {
        if (!scopeOptionWarned) {
          console.warn("`scope` option is deprecated and will be removed in EJS 3");
          scopeOptionWarned = true
        }
        if (!opts.context) {
          opts.context = opts.scope
        }
        delete opts.scope
      }
      templ = new Template(template, opts);
      return templ.compile()
    };
    exports.render = function (template, data, opts) {
      data = data || {};
      opts = opts || {};
      var fn;
      if (arguments.length == 2) {
        cpOptsInData(data, opts)
      }
      return handleCache(opts, template)(data)
    };
    exports.renderFile = function () {
      var args = Array.prototype.slice.call(arguments),
        path = args.shift(),
        cb = args.pop(),
        data = args.shift() || {},
        opts = args.pop() || {},
        result;
      opts = utils.shallowCopy({}, opts);
      if (arguments.length == 3) {
        if (data.settings && data.settings["view options"]) {
          cpOptsInData(data.settings["view options"], opts)
        } else {
          cpOptsInData(data, opts)
        }
      }
      opts.filename = path;
      try {
        result = handleCache(opts)(data)
      } catch (err) {
        return cb(err)
      }
      return cb(null, result)
    };
    exports.clearCache = function () {
      exports.cache.reset()
    };

    function Template(text, opts) {
      opts = opts || {};
      var options = {};
      this.templateText = text;
      this.mode = null;
      this.truncate = false;
      this.currentLine = 1;
      this.source = "";
      this.dependencies = [];
      options.client = opts.client || false;
      options.escapeFunction = opts.escape || utils.escapeXML;
      options.compileDebug = opts.compileDebug !== false;
      options.debug = !!opts.debug;
      options.filename = opts.filename;
      options.delimiter = opts.delimiter || exports.delimiter || _DEFAULT_DELIMITER;
      options.strict = opts.strict || false;
      options.context = opts.context;
      options.cache = opts.cache || false;
      options.rmWhitespace = opts.rmWhitespace;
      options.localsName = opts.localsName || exports.localsName || _DEFAULT_LOCALS_NAME;
      if (options.strict) {
        options._with = false
      } else {
        options._with = typeof opts._with != "undefined" ? opts._with : true
      }
      this.opts = options;
      this.regex = this.createRegex()
    }
    Template.modes = {
      EVAL: "eval",
      ESCAPED: "escaped",
      RAW: "raw",
      COMMENT: "comment",
      LITERAL: "literal"
    };
    Template.prototype = {
      createRegex: function () {
        var str = _REGEX_STRING,
          delim = utils.escapeRegExpChars(this.opts.delimiter);
        str = str.replace(/%/g, delim);
        return new RegExp(str)
      },
      compile: function () {
        var src, fn, opts = this.opts,
          prepended = "",
          appended = "",
          escape = opts.escapeFunction;
        if (opts.rmWhitespace) {
          this.templateText = this.templateText.replace(/\r/g, "").replace(/^\s+|\s+$/gm, "")
        }
        this.templateText = this.templateText.replace(/[ \t]*<%_/gm, "<%_").replace(/_%>[ \t]*/gm, "_%>");
        if (!this.source) {
          this.generateSource();
          prepended += "  var __output = [], __append = __output.push.bind(__output);" + "\n";
          if (opts._with !== false) {
            prepended += "  with (" + opts.localsName + " || {}) {" + "\n";
            appended += "  }" + "\n"
          }
          appended += '  return __output.join("");' + "\n";
          this.source = prepended + this.source + appended
        }
        if (opts.compileDebug) {
          src = "var __line = 1" + "\n" + "  , __lines = " + JSON.stringify(this.templateText) + "\n" + "  , __filename = " + (opts.filename ? JSON.stringify(opts.filename) : "undefined") + ";" + "\n" + "try {" + "\n" + this.source + "} catch (e) {" + "\n" + "  rethrow(e, __lines, __filename, __line);" + "\n" + "}" + "\n"
        } else {
          src = this.source
        }
        if (opts.debug) {
          console.log(src)
        }
        if (opts.client) {
          src = "escape = escape || " + escape.toString() + ";" + "\n" + src;
          if (opts.compileDebug) {
            src = "rethrow = rethrow || " + rethrow.toString() + ";" + "\n" + src
          }
        }
        if (opts.strict) {
          src = '"use strict";\n' + src
        }
        try {
          fn = new Function(opts.localsName + ", escape, include, rethrow", src)
        } catch (e) {
          if (e instanceof SyntaxError) {
            if (opts.filename) {
              e.message += " in " + opts.filename
            }
            e.message += " while compiling ejs"
          }
          throw e
        }
        if (opts.client) {
          fn.dependencies = this.dependencies;
          return fn
        }
        var returnedFn = function (data) {
          var include = function (path, includeData) {
            var d = utils.shallowCopy({}, data);
            if (includeData) {
              d = utils.shallowCopy(d, includeData)
            }
            return includeFile(path, opts)(d)
          };
          return fn.apply(opts.context, [data || {}, escape, include, rethrow])
        };
        returnedFn.dependencies = this.dependencies;
        return returnedFn
      },
      generateSource: function () {
        var self = this,
          matches = this.parseTemplateText(),
          d = this.opts.delimiter;
        if (matches && matches.length) {
          matches.forEach(function (line, index) {
            var opening, closing, include, includeOpts, includeSrc;
            if (line.indexOf("<" + d) === 0 && line.indexOf("<" + d + d) !== 0) {
              closing = matches[index + 2];
              if (!(closing == d + ">" || closing == "-" + d + ">" || closing == "_" + d + ">")) {
                throw new Error('Could not find matching close tag for "' + line + '".')
              }
            }
            if (include = line.match(/^\s*include\s+(\S+)/)) {
              opening = matches[index - 1];
              if (opening && (opening == "<" + d || opening == "<" + d + "-" || opening == "<" + d + "_")) {
                includeOpts = utils.shallowCopy({}, self.opts);
                includeSrc = includeSource(include[1], includeOpts);
                includeSrc = "    ; (function(){" + "\n" + includeSrc + "    ; })()" + "\n";
                self.source += includeSrc;
                self.dependencies.push(exports.resolveInclude(include[1], includeOpts.filename));
                return
              }
            }
            self.scanLine(line)
          })
        }
      },
      parseTemplateText: function () {
        var str = this.templateText,
          pat = this.regex,
          result = pat.exec(str),
          arr = [],
          firstPos, lastPos;
        while (result) {
          firstPos = result.index;
          lastPos = pat.lastIndex;
          if (firstPos !== 0) {
            arr.push(str.substring(0, firstPos));
            str = str.slice(firstPos)
          }
          arr.push(result[0]);
          str = str.slice(result[0].length);
          result = pat.exec(str)
        }
        if (str) {
          arr.push(str)
        }
        return arr
      },
      scanLine: function (line) {
        var self = this,
          d = this.opts.delimiter,
          newLineCount = 0;

        function _addOutput() {
          if (self.truncate) {
            line = line.replace(/^(?:\r\n|\r|\n)/, "");
            self.truncate = false
          } else if (self.opts.rmWhitespace) {
            line = line.replace(/^\n/, "")
          }
          if (!line) {
            return
          }
          line = line.replace(/\\/g, "\\\\");
          line = line.replace(/\n/g, "\\n");
          line = line.replace(/\r/g, "\\r");
          line = line.replace(/"/g, '\\"');
          self.source += '    ; __append("' + line + '")' + "\n"
        }
        newLineCount = line.split("\n").length - 1;
        switch (line) {
        case "<" + d:
        case "<" + d + "_":
          this.mode = Template.modes.EVAL;
          break;
        case "<" + d + "=":
          this.mode = Template.modes.ESCAPED;
          break;
        case "<" + d + "-":
          this.mode = Template.modes.RAW;
          break;
        case "<" + d + "#":
          this.mode = Template.modes.COMMENT;
          break;
        case "<" + d + d:
          this.mode = Template.modes.LITERAL;
          this.source += '    ; __append("' + line.replace("<" + d + d, "<" + d) + '")' + "\n";
          break;
        case d + ">":
        case "-" + d + ">":
        case "_" + d + ">":
          if (this.mode == Template.modes.LITERAL) {
            _addOutput()
          }
          this.mode = null;
          this.truncate = line.indexOf("-") === 0 || line.indexOf("_") === 0;
          break;
        default:
          if (this.mode) {
            switch (this.mode) {
            case Template.modes.EVAL:
            case Template.modes.ESCAPED:
            case Template.modes.RAW:
              if (line.lastIndexOf("//") > line.lastIndexOf("\n")) {
                line += "\n"
              }
            }
            switch (this.mode) {
            case Template.modes.EVAL:
              this.source += "    ; " + line + "\n";
              break;
            case Template.modes.ESCAPED:
              this.source += "    ; __append(escape(" + line.replace(_TRAILING_SEMCOL, "").trim() + "))" + "\n";
              break;
            case Template.modes.RAW:
              this.source += "    ; __append(" + line.replace(_TRAILING_SEMCOL, "").trim() + ")" + "\n";
              break;
            case Template.modes.COMMENT:
              break;
            case Template.modes.LITERAL:
              _addOutput();
              break
            }
          } else {
            _addOutput()
          }
        }
        if (self.opts.compileDebug && newLineCount) {
          this.currentLine += newLineCount;
          this.source += "    ; __line = " + this.currentLine + "\n"
        }
      }
    };
    exports.__express = exports.renderFile;
    if (require.extensions) {
      require.extensions[".ejs"] = function (module, filename) {
        filename = filename || module.filename;
        var options = {
            filename: filename,
            client: true
          },
          template = fs.readFileSync(filename).toString(),
          fn = exports.compile(template, options);
        module._compile("module.exports = " + fn.toString() + ";", filename)
      }
    }
    exports.VERSION = _VERSION_STRING;
    if (typeof window != "undefined") {
      window.ejs = exports
    }
  }, {
    "../package.json": 6,
    "./utils": 2,
    fs: 3,
    path: 4
  }],
  2: [function (require, module, exports) {
    "use strict";
    var regExpChars = /[|\\{}()[\]^$+*?.]/g;
    exports.escapeRegExpChars = function (string) {
      if (!string) {
        return ""
      }
      return String(string).replace(regExpChars, "\\$&")
    };
    var _ENCODE_HTML_RULES = {
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&#34;",
        "'": "&#39;"
      },
      _MATCH_HTML = /[&<>\'"]/g;

    function encode_char(c) {
      return _ENCODE_HTML_RULES[c] || c
    }
    var escapeFuncStr = "var _ENCODE_HTML_RULES = {\n" + '      "&": "&amp;"\n' + '    , "<": "&lt;"\n' + '    , ">": "&gt;"\n' + '    , \'"\': "&#34;"\n' + '    , "\'": "&#39;"\n' + "    }\n" + "  , _MATCH_HTML = /[&<>'\"]/g;\n" + "function encode_char(c) {\n" + "  return _ENCODE_HTML_RULES[c] || c;\n" + "};\n";
    exports.escapeXML = function (markup) {
      return markup == undefined ? "" : String(markup).replace(_MATCH_HTML, encode_char)
    };
    exports.escapeXML.toString = function () {
      return Function.prototype.toString.call(this) + ";\n" + escapeFuncStr
    };
    exports.shallowCopy = function (to, from) {
      from = from || {};
      for (var p in from) {
        to[p] = from[p]
      }
      return to
    };
    exports.cache = {
      _data: {},
      set: function (key, val) {
        this._data[key] = val
      },
      get: function (key) {
        return this._data[key]
      },
      reset: function () {
        this._data = {}
      }
    }
  }, {}],
  3: [function (require, module, exports) {}, {}],
  4: [function (require, module, exports) {
    (function (process) {
      function normalizeArray(parts, allowAboveRoot) {
        var up = 0;
        for (var i = parts.length - 1; i >= 0; i--) {
          var last = parts[i];
          if (last === ".") {
            parts.splice(i, 1)
          } else if (last === "..") {
            parts.splice(i, 1);
            up++
          } else if (up) {
            parts.splice(i, 1);
            up--
          }
        }
        if (allowAboveRoot) {
          for (; up--; up) {
            parts.unshift("..")
          }
        }
        return parts
      }
      var splitPathRe = /^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/;
      var splitPath = function (filename) {
        return splitPathRe.exec(filename).slice(1)
      };
      exports.resolve = function () {
        var resolvedPath = "",
          resolvedAbsolute = false;
        for (var i = arguments.length - 1; i >= -1 && !resolvedAbsolute; i--) {
          var path = i >= 0 ? arguments[i] : process.cwd();
          if (typeof path !== "string") {
            throw new TypeError("Arguments to path.resolve must be strings")
          } else if (!path) {
            continue
          }
          resolvedPath = path + "/" + resolvedPath;
          resolvedAbsolute = path.charAt(0) === "/"
        }
        resolvedPath = normalizeArray(filter(resolvedPath.split("/"), function (p) {
          return !!p
        }), !resolvedAbsolute).join("/");
        return (resolvedAbsolute ? "/" : "") + resolvedPath || "."
      };
      exports.normalize = function (path) {
        var isAbsolute = exports.isAbsolute(path),
          trailingSlash = substr(path, -1) === "/";
        path = normalizeArray(filter(path.split("/"), function (p) {
          return !!p
        }), !isAbsolute).join("/");
        if (!path && !isAbsolute) {
          path = "."
        }
        if (path && trailingSlash) {
          path += "/"
        }
        return (isAbsolute ? "/" : "") + path
      };
      exports.isAbsolute = function (path) {
        return path.charAt(0) === "/"
      };
      exports.join = function () {
        var paths = Array.prototype.slice.call(arguments, 0);
        return exports.normalize(filter(paths, function (p, index) {
          if (typeof p !== "string") {
            throw new TypeError("Arguments to path.join must be strings")
          }
          return p
        }).join("/"))
      };
      exports.relative = function (from, to) {
        from = exports.resolve(from).substr(1);
        to = exports.resolve(to).substr(1);

        function trim(arr) {
          var start = 0;
          for (; start < arr.length; start++) {
            if (arr[start] !== "") break
          }
          var end = arr.length - 1;
          for (; end >= 0; end--) {
            if (arr[end] !== "") break
          }
          if (start > end) return [];
          return arr.slice(start, end - start + 1)
        }
        var fromParts = trim(from.split("/"));
        var toParts = trim(to.split("/"));
        var length = Math.min(fromParts.length, toParts.length);
        var samePartsLength = length;
        for (var i = 0; i < length; i++) {
          if (fromParts[i] !== toParts[i]) {
            samePartsLength = i;
            break
          }
        }
        var outputParts = [];
        for (var i = samePartsLength; i < fromParts.length; i++) {
          outputParts.push("..")
        }
        outputParts = outputParts.concat(toParts.slice(samePartsLength));
        return outputParts.join("/")
      };
      exports.sep = "/";
      exports.delimiter = ":";
      exports.dirname = function (path) {
        var result = splitPath(path),
          root = result[0],
          dir = result[1];
        if (!root && !dir) {
          return "."
        }
        if (dir) {
          dir = dir.substr(0, dir.length - 1)
        }
        return root + dir
      };
      exports.basename = function (path, ext) {
        var f = splitPath(path)[2];
        if (ext && f.substr(-1 * ext.length) === ext) {
          f = f.substr(0, f.length - ext.length)
        }
        return f
      };
      exports.extname = function (path) {
        return splitPath(path)[3]
      };

      function filter(xs, f) {
        if (xs.filter) return xs.filter(f);
        var res = [];
        for (var i = 0; i < xs.length; i++) {
          if (f(xs[i], i, xs)) res.push(xs[i])
        }
        return res
      }
      var substr = "ab".substr(-1) === "b" ? function (str, start, len) {
        return str.substr(start, len)
      } : function (str, start, len) {
        if (start < 0) start = str.length + start;
        return str.substr(start, len)
      }
    }).call(this, require("_process"))
  }, {
    _process: 5
  }],
  5: [function (require, module, exports) {
    var process = module.exports = {};
    var queue = [];
    var draining = false;

    function drainQueue() {
      if (draining) {
        return
      }
      draining = true;
      var currentQueue;
      var len = queue.length;
      while (len) {
        currentQueue = queue;
        queue = [];
        var i = -1;
        while (++i < len) {
          currentQueue[i]()
        }
        len = queue.length
      }
      draining = false
    }
    process.nextTick = function (fun) {
      queue.push(fun);
      if (!draining) {
        setTimeout(drainQueue, 0)
      }
    };
    process.title = "browser";
    process.browser = true;
    process.env = {};
    process.argv = [];
    process.version = "";

    function noop() {}
    process.on = noop;
    process.addListener = noop;
    process.once = noop;
    process.off = noop;
    process.removeListener = noop;
    process.removeAllListeners = noop;
    process.emit = noop;
    process.binding = function (name) {
      throw new Error("process.binding is not supported")
    };
    process.cwd = function () {
      return "/"
    };
    process.chdir = function (dir) {
      throw new Error("process.chdir is not supported")
    };
    process.umask = function () {
      return 0
    }
  }, {}],
  6: [function (require, module, exports) {
    module.exports = {
      name: "ejs",
      description: "Embedded JavaScript templates",
      keywords: ["template", "engine", "ejs"],
      version: "2.4.1",
      author: "Matthew Eernisse <mde@fleegix.org> (http://fleegix.org)",
      contributors: ["Timothy Gu <timothygu99@gmail.com> (https://timothygu.github.io)"],
      license: "Apache-2.0",
      main: "./lib/ejs.js",
      repository: {
        type: "git",
        url: "git://github.com/mde/ejs.git"
      },
      bugs: "https://github.com/mde/ejs/issues",
      homepage: "https://github.com/mde/ejs",
      dependencies: {},
      devDependencies: {
        browserify: "^8.0.3",
        istanbul: "~0.3.5",
        jake: "^8.0.0",
        jsdoc: "^3.3.0-beta1",
        "lru-cache": "^2.5.0",
        mocha: "^2.1.0",
        rimraf: "^2.2.8",
        "uglify-js": "^2.4.16"
      },
      engines: {
        node: ">=0.10.0"
      },
      scripts: {
        test: "mocha",
        coverage: "istanbul cover node_modules/mocha/bin/_mocha",
        doc: "rimraf out && jsdoc -c jsdoc.json lib/* docs/jsdoc/*",
        devdoc: "rimraf out && jsdoc -p -c jsdoc.json lib/* docs/jsdoc/*"
      }
    }
  }, {}]
}, {}, [1]);
! function (t, e) {
  function n(t, n) {
    function r() {
      E.hasOwnProperty(_) && delete E[_], x = null
    }

    function a() {
      var t = 0;
      for (var e in x.tables) x.tables.hasOwnProperty(e) && t++;
      return t
    }

    function i(t) {
      return x.tables[t].fields
    }

    function o(t) {
      return x.tables[t] ? !0 : !1
    }

    function f(t) {
      o(t) || D("The table '" + t + "' does not exist")
    }

    function u(t, e) {
      var n = !1,
        r = x.tables[t].fields;
      for (var a in r)
        if (r[a] == e) {
          n = !0;
          break
        }
      return n
    }

    function l(t, e) {
      x.tables[t] = {
        fields: e,
        auto_increment: 1
      }, x.data[t] = {}
    }

    function s(t) {
      delete x.tables[t], delete x.data[t]
    }

    function c(t) {
      x.tables[t].auto_increment = 1, x.data[t] = {}
    }

    function d(t, e, n) {
      if (x.tables[t].fields = x.tables[t].fields.concat(e), "undefined" != typeof n)
        for (var r in x.data[t])
          if (x.data[t].hasOwnProperty(r))
            for (var a in e) x.data[t][r][e[a]] = "object" == typeof n ? n[e[a]] : n
    }

    function h(t) {
      var e = 0;
      for (var n in x.data[t]) x.data[t].hasOwnProperty(n) && e++;
      return e
    }

    function v(t, e) {
      return e.ID = x.tables[t].auto_increment, x.data[t][x.tables[t].auto_increment] = e, x.tables[t].auto_increment++, e.ID
    }

    function p(t, n, r, a, i, o) {
      for (var f = null, u = [], l = null, s = 0; s < n.length; s++) f = n[s], l = x.data[t][f], u.push(k(l));
      if (i && i instanceof Array)
        for (var s = 0; s < i.length; s++) u.sort(y(i[s][0], i[s].length > 1 ? i[s][1] : null));
      if (o && o instanceof Array) {
        for (var c = 0; c < o.length; c++)
          for (var d = {}, h = o[c], s = 0; s < u.length; s++) u[s] !== e && (u[s].hasOwnProperty(h) && d.hasOwnProperty(u[s][h]) ? delete u[s] : d[u[s][h]] = 1);
        for (var v = [], s = 0; s < u.length; s++) u[s] !== e && v.push(u[s]);
        u = v
      }
      return r = r && "number" == typeof r ? r : null, a = a && "number" == typeof a ? a : null, r && a ? u = u.slice(r, r + a) : r ? u = u.slice(r) : a && (u = u.slice(r, a)), u
    }

    function y(t, e) {
      return function (n, r) {
        var a = "string" == typeof n[t] ? n[t].toLowerCase() : n[t],
          i = "string" == typeof r[t] ? r[t].toLowerCase() : r[t];
        return "DESC" === e ? a == i ? 0 : i > a ? 1 : -1 : a == i ? 0 : a > i ? 1 : -1
      }
    }

    function b(t, e) {
      var n = [],
        r = !1,
        a = null;
      for (var i in x.data[t])
        if (x.data[t].hasOwnProperty(i)) {
          a = x.data[t][i], r = !0;
          for (var o in e)
            if (e.hasOwnProperty(o))
              if ("string" == typeof e[o]) {
                if (a[o].toString().toLowerCase() != e[o].toString().toLowerCase()) {
                  r = !1;
                  break
                }
              } else if (a[o] != e[o]) {
            r = !1;
            break
          }
          r && n.push(i)
        }
      return n
    }

    function g(t, e) {
      var n = [],
        r = null;
      for (var a in x.data[t]) x.data[t].hasOwnProperty(a) && (r = x.data[t][a], 1 == e(k(r)) && n.push(a));
      return n
    }

    function m(t) {
      var e = [];
      for (var n in x.data[t]) x.data[t].hasOwnProperty(n) && e.push(n);
      return e
    }

    function w(t, e) {
      for (var n = 0; n < e.length; n++) x.data[t].hasOwnProperty(e[n]) && delete x.data[t][e[n]];
      return e.length
    }

    function O(t, e, n) {
      for (var r = "", a = 0, i = 0; i < e.length; i++) {
        r = e[i];
        var o = n(k(x.data[t][r]));
        if (o) {
          delete o.ID;
          var f = x.data[t][r];
          for (var u in o) o.hasOwnProperty(u) && (f[u] = o[u]);
          x.data[t][r] = j(t, f), a++
        }
      }
      return a
    }

    function P() {
      try {
        return E.setItem(_, JSON.stringify(x)), !0
      } catch (t) {
        return !1
      }
    }

    function S() {
      return JSON.stringify(x)
    }

    function D(t) {
      throw new Error(t)
    }

    function k(t) {
      var e = {};
      for (var n in t) t.hasOwnProperty(n) && (e[n] = t[n]);
      return e
    }

    function T(t) {
      return t.toString().match(/[^a-z_0-9]/gi) ? !1 : !0
    }

    function j(t, n) {
      for (var r = "", a = {}, i = 0; i < x.tables[t].fields.length; i++) r = x.tables[t].fields[i], n[r] !== e && (a[r] = n[r]);
      return a
    }

    function I(t, n) {
      for (var r = "", a = {}, i = 0; i < x.tables[t].fields.length; i++) r = x.tables[t].fields[i], a[r] = null === n[r] || n[r] === e ? null : n[r];
      return a
    }
    var C = "db_",
      _ = C + t,
      q = !1,
      x = null;
    try {
      var E = n == sessionStorage ? sessionStorage : localStorage
    } catch (N) {
      var E = n
    }
    return x = E[_], x && (x = JSON.parse(x)) && x.tables && x.data || (T(t) ? (x = {
      tables: {},
      data: {}
    }, P(), q = !0) : D("The name '" + t + "' contains invalid characters")), {
      commit: function () {
        return P()
      },
      isNew: function () {
        return q
      },
      drop: function () {
        r()
      },
      serialize: function () {
        return S()
      },
      tableExists: function (t) {
        return o(t)
      },
      tableFields: function (t) {
        return i(t)
      },
      tableCount: function () {
        return a()
      },
      columnExists: function (t, e) {
        return u(t, e)
      },
      createTable: function (t, e) {
        var n = !1;
        if (T(t))
          if (this.tableExists(t)) D("The table name '" + t + "' already exists.");
          else {
            for (var r = !0, a = 0; a < e.length; a++)
              if (!T(e[a])) {
                r = !1;
                break
              }
            if (r) {
              for (var i = {}, a = 0; a < e.length; a++) i[e[a]] = !0;
              delete i.ID, e = ["ID"];
              for (var o in i) i.hasOwnProperty(o) && e.push(o);
              l(t, e), n = !0
            } else D("One or more field names in the table definition contains invalid characters")
          }
        else D("The database name '" + t + "' contains invalid characters.");
        return n
      },
      createTableWithData: function (t, e) {
        ("object" != typeof e || !e.length || e.length < 1) && D("Data supplied isn't in object form. Example: [{k:v,k:v},{k:v,k:v} ..]");
        var n = Object.keys(e[0]);
        if (this.createTable(t, n)) {
          this.commit();
          for (var r = 0; r < e.length; r++) v(t, e[r]) || D("Failed to insert record: [" + JSON.stringify(e[r]) + "]");
          this.commit()
        }
        return !0
      },
      dropTable: function (t) {
        f(t), s(t)
      },
      truncate: function (t) {
        f(t), c(t)
      },
      alterTable: function (t, e, n) {
        var r = !1;
        if (T(t)) {
          if ("object" == typeof e) {
            for (var a = !0, i = 0; i < e.length; i++)
              if (!T(e[i])) {
                a = !1;
                break
              }
            if (a) {
              for (var o = {}, i = 0; i < e.length; i++) o[e[i]] = !0;
              delete o.ID, e = [];
              for (var f in o) o.hasOwnProperty(f) && e.push(f);
              d(t, e, n), r = !0
            } else D("One or more field names in the table definition contains invalid characters")
          } else if ("string" == typeof e)
            if (T(e)) {
              var u = [];
              u.push(e), d(t, u, n), r = !0
            } else D("One or more field names in the table definition contains invalid characters")
        } else D("The database name '" + t + "' contains invalid characters");
        return r
      },
      rowCount: function (t) {
        return f(t), h(t)
      },
      insert: function (t, e) {
        return f(t), v(t, I(t, e))
      },
      insertOrUpdate: function (t, e, n) {
        f(t);
        var r = [];
        if (e ? "object" == typeof e ? r = b(t, j(t, e)) : "function" == typeof e && (r = g(t, e)) : r = m(t), 0 == r.length) return v(t, I(t, n));
        for (var a = [], i = 0; i < r.length; i++) O(t, r, function (t) {
          return a.push(t.ID), n
        });
        return a
      },
      update: function (t, e, n) {
        f(t);
        var r = [];
        return e ? "object" == typeof e ? r = b(t, j(t, e)) : "function" == typeof e && (r = g(t, e)) : r = m(t), O(t, r, n)
      },
      query: function (t, e, n, r, a, i) {
        f(t);
        var o = [];
        return e ? "object" == typeof e ? o = b(t, j(t, e), n, r) : "function" == typeof e && (o = g(t, e, n, r)) : o = m(t, n, r), p(t, o, r, n, a, i)
      },
      queryAll: function (t, e) {
        return e ? this.query(t, e.hasOwnProperty("query") ? e.query : null, e.hasOwnProperty("limit") ? e.limit : null, e.hasOwnProperty("start") ? e.start : null, e.hasOwnProperty("sort") ? e.sort : null, e.hasOwnProperty("distinct") ? e.distinct : null) : this.query(t)
      },
      deleteRows: function (t, e) {
        f(t);
        var n = [];
        return e ? "object" == typeof e ? n = b(t, j(t, e)) : "function" == typeof e && (n = g(t, e)) : n = m(t), w(t, n)
      }
    }
  }
  "function" == typeof define && define.amd ? define(function () {
    return n
  }) : t.localStorageDB = n
}(window);
(function (d) {
  "function" === typeof define && define.amd ? define(["jquery"], d) : d(jQuery)
})(function (d) {
  function k(a) {
    return e.raw ? a : decodeURIComponent(a.replace(n, " "))
  }

  function l(a) {
    0 === a.indexOf('"') && (a = a.slice(1, -1).replace(/\\"/g, '"').replace(/\\\\/g, "\\"));
    a = k(a);
    try {
      return e.json ? JSON.parse(a) : a
    } catch (c) {}
  }
  var n = /\+/g,
    e = d.cookie = function (a, c, b) {
      if (void 0 !== c) {
        b = d.extend({}, e.defaults, b);
        if ("number" === typeof b.expires) {
          var f = b.expires,
            h = b.expires = new Date;
          h.setDate(h.getDate() + f)
        }
        c = e.json ? JSON.stringify(c) : String(c);
        return document.cookie = [e.raw ? a : encodeURIComponent(a), "=", e.raw ? c : encodeURIComponent(c), b.expires ? "; expires=" + b.expires.toUTCString() : "", b.path ? "; path=" + b.path : "", b.domain ? "; domain=" + b.domain : "", b.secure ? "; secure" : ""].join("")
      }
      c = document.cookie.split("; ");
      b = a ? void 0 : {};
      f = 0;
      for (h = c.length; f < h; f++) {
        var g = c[f].split("="),
          m = k(g.shift()),
          g = g.join("=");
        if (a && a === m) {
          b = l(g);
          break
        }
        a || (b[m] = l(g))
      }
      return b
    };
  e.defaults = {};
  d.removeCookie = function (a, c) {
    return void 0 !== d.cookie(a) ? (d.cookie(a, "", d.extend({}, c, {
      expires: -1
    })), !0) : !1
  }
});
'use strict';
(function (E) {
  function n(c, b, f) {
    var a = 0,
      d = [0],
      e = "",
      g = null,
      e = f || "UTF8";
    if ("UTF8" !== e && "UTF16BE" !== e && "UTF16LE" !== e) throw "encoding must be UTF8, UTF16BE, or UTF16LE";
    if ("HEX" === b) {
      if (0 !== c.length % 2) throw "srcString of HEX type must be in byte increments";
      g = w(c);
      a = g.binLen;
      d = g.value
    } else if ("TEXT" === b || "ASCII" === b) g = x(c, e), a = g.binLen, d = g.value;
    else if ("B64" === b) g = y(c), a = g.binLen, d = g.value;
    else if ("BYTES" === b) g = z(c), a = g.binLen, d = g.value;
    else throw "inputFormat must be HEX, TEXT, ASCII, B64, or BYTES";
    this.getHash = function (c, b, e, f) {
      var g = null,
        h = d.slice(),
        m = a,
        p;
      3 === arguments.length ? "number" !== typeof e && (f = e, e = 1) : 2 === arguments.length && (e = 1);
      if (e !== parseInt(e, 10) || 1 > e) throw "numRounds must a integer >= 1";
      switch (b) {
      case "HEX":
        g = A;
        break;
      case "B64":
        g = B;
        break;
      case "BYTES":
        g = C;
        break;
      default:
        throw "format must be HEX, B64, or BYTES";
      }
      if ("SHA-1" === c)
        for (p = 0; p < e; p += 1) h = u(h, m), m = 160;
      else throw "Chosen SHA variant is not supported";
      return g(h, D(f))
    };
    this.getHMAC = function (c, b, f, g, r) {
      var h, m, p, t, n = [],
        v = [];
      h = null;
      switch (g) {
      case "HEX":
        g = A;
        break;
      case "B64":
        g = B;
        break;
      case "BYTES":
        g = C;
        break;
      default:
        throw "outputFormat must be HEX, B64, or BYTES";
      }
      if ("SHA-1" === f) m = 64, t = 160;
      else throw "Chosen SHA variant is not supported";
      if ("HEX" === b) h = w(c), p = h.binLen, h = h.value;
      else if ("TEXT" === b || "ASCII" === b) h = x(c, e), p = h.binLen, h = h.value;
      else if ("B64" === b) h = y(c), p = h.binLen, h = h.value;
      else if ("BYTES" === b) h = z(c), p = h.binLen, h = h.value;
      else throw "inputFormat must be HEX, TEXT, ASCII, B64, or BYTES";
      c = 8 * m;
      b = m / 4 - 1;
      if (m < p / 8) {
        if ("SHA-1" === f) h = u(h, p);
        else throw "Unexpected error in HMAC implementation";
        for (; h.length <= b;) h.push(0);
        h[b] &= 4294967040
      } else if (m > p / 8) {
        for (; h.length <= b;) h.push(0);
        h[b] &= 4294967040
      }
      for (m = 0; m <= b; m += 1) n[m] = h[m] ^ 909522486, v[m] = h[m] ^ 1549556828;
      if ("SHA-1" === f) f = u(v.concat(u(n.concat(d), c + a)), c + t);
      else throw "Unexpected error in HMAC implementation";
      return g(f, D(r))
    }
  }

  function x(c, b) {
    var f = [],
      a, d = [],
      e = 0,
      g, k, q;
    if ("UTF8" === b)
      for (g = 0; g < c.length; g += 1)
        for (a = c.charCodeAt(g), d = [], 128 > a ? d.push(a) : 2048 > a ? (d.push(192 | a >>> 6), d.push(128 | a & 63)) : 55296 > a || 57344 <= a ? d.push(224 | a >>> 12, 128 | a >>> 6 & 63, 128 | a & 63) : (g += 1, a = 65536 + ((a & 1023) << 10 | c.charCodeAt(g) & 1023), d.push(240 | a >>> 18, 128 | a >>> 12 & 63, 128 | a >>> 6 & 63, 128 | a & 63)), k = 0; k < d.length; k += 1) {
          for (q = e >>> 2; f.length <= q;) f.push(0);
          f[q] |= d[k] << 24 - e % 4 * 8;
          e += 1
        } else if ("UTF16BE" === b || "UTF16LE" === b)
          for (g = 0; g < c.length; g += 1) {
            a = c.charCodeAt(g);
            "UTF16LE" === b && (k = a & 255, a = k << 8 | a >> 8);
            for (q = e >>> 2; f.length <= q;) f.push(0);
            f[q] |= a << 16 - e % 4 * 8;
            e += 2
          }
    return {
      value: f,
      binLen: 8 * e
    }
  }

  function w(c) {
    var b = [],
      f = c.length,
      a, d, e;
    if (0 !== f % 2) throw "String of HEX type must be in byte increments";
    for (a = 0; a < f; a += 2) {
      d = parseInt(c.substr(a, 2), 16);
      if (isNaN(d)) throw "String of HEX type contains invalid characters";
      for (e = a >>> 3; b.length <= e;) b.push(0);
      b[a >>> 3] |= d << 24 - a % 8 * 4
    }
    return {
      value: b,
      binLen: 4 * f
    }
  }

  function z(c) {
    var b = [],
      f, a, d;
    for (a = 0; a < c.length; a += 1) f = c.charCodeAt(a), d = a >>> 2, b.length <= d && b.push(0), b[d] |= f << 24 - a % 4 * 8;
    return {
      value: b,
      binLen: 8 * c.length
    }
  }

  function y(c) {
    var b = [],
      f = 0,
      a, d, e, g, k;
    if (-1 === c.search(/^[a-zA-Z0-9=+\/]+$/)) throw "Invalid character in base-64 string";
    d = c.indexOf("=");
    c = c.replace(/\=/g, "");
    if (-1 !== d && d < c.length) throw "Invalid '=' found in base-64 string";
    for (d = 0; d < c.length; d += 4) {
      k = c.substr(d, 4);
      for (e = g = 0; e < k.length; e += 1) a = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".indexOf(k[e]), g |= a << 18 - 6 * e;
      for (e = 0; e < k.length - 1; e += 1) {
        for (a = f >>> 2; b.length <= a;) b.push(0);
        b[a] |= (g >>> 16 - 8 * e & 255) << 24 - f % 4 * 8;
        f += 1
      }
    }
    return {
      value: b,
      binLen: 8 * f
    }
  }

  function A(c, b) {
    var f = "",
      a = 4 * c.length,
      d, e;
    for (d = 0; d < a; d += 1) e = c[d >>> 2] >>> 8 * (3 - d % 4), f += "0123456789abcdef".charAt(e >>> 4 & 15) + "0123456789abcdef".charAt(e & 15);
    return b.outputUpper ? f.toUpperCase() : f
  }

  function B(c, b) {
    var f = "",
      a = 4 * c.length,
      d, e, g;
    for (d = 0; d < a; d += 3)
      for (g = d + 1 >>> 2, e = c.length <= g ? 0 : c[g], g = d + 2 >>> 2, g = c.length <= g ? 0 : c[g], g = (c[d >>> 2] >>> 8 * (3 - d % 4) & 255) << 16 | (e >>> 8 * (3 - (d + 1) % 4) & 255) << 8 | g >>> 8 * (3 - (d + 2) % 4) & 255, e = 0; 4 > e; e += 1) 8 * d + 6 * e <= 32 * c.length ? f += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".charAt(g >>> 6 * (3 - e) & 63) : f += b.b64Pad;
    return f
  }

  function C(c) {
    var b = "",
      f = 4 * c.length,
      a, d;
    for (a = 0; a < f; a += 1) d = c[a >>> 2] >>> 8 * (3 - a % 4) & 255, b += String.fromCharCode(d);
    return b
  }

  function D(c) {
    var b = {
      outputUpper: !1,
      b64Pad: "="
    };
    try {
      c.hasOwnProperty("outputUpper") && (b.outputUpper = c.outputUpper), c.hasOwnProperty("b64Pad") && (b.b64Pad = c.b64Pad)
    } catch (f) {}
    if ("boolean" !== typeof b.outputUpper) throw "Invalid outputUpper formatting option";
    if ("string" !== typeof b.b64Pad) throw "Invalid b64Pad formatting option";
    return b
  }

  function r(c, b) {
    return c << b | c >>> 32 - b
  }

  function t(c, b) {
    var f = (c & 65535) + (b & 65535);
    return ((c >>> 16) + (b >>> 16) + (f >>> 16) & 65535) << 16 | f & 65535
  }

  function v(c, b, f, a, d) {
    var e = (c & 65535) + (b & 65535) + (f & 65535) + (a & 65535) + (d & 65535);
    return ((c >>> 16) + (b >>> 16) + (f >>> 16) + (a >>> 16) + (d >>> 16) + (e >>> 16) & 65535) << 16 | e & 65535
  }

  function u(c, b) {
    var f = [],
      a, d, e, g, k, q, n, l, u, h = [1732584193, 4023233417, 2562383102, 271733878, 3285377520];
    for (a = (b + 65 >>> 9 << 4) + 15; c.length <= a;) c.push(0);
    c[b >>> 5] |= 128 << 24 - b % 32;
    c[a] = b & 4294967295;
    c[a - 1] = b / 4294967296 | 0;
    u = c.length;
    for (n = 0; n < u; n += 16) {
      a = h[0];
      d = h[1];
      e = h[2];
      g = h[3];
      k = h[4];
      for (l = 0; 80 > l; l += 1) f[l] = 16 > l ? c[l + n] : r(f[l - 3] ^ f[l - 8] ^ f[l - 14] ^ f[l - 16], 1), q = 20 > l ? v(r(a, 5), d & e ^ ~d & g, k, 1518500249, f[l]) : 40 > l ? v(r(a, 5), d ^ e ^ g, k, 1859775393, f[l]) : 60 > l ? v(r(a, 5), d & e ^ d & g ^ e & g, k, 2400959708, f[l]) : v(r(a, 5), d ^ e ^ g, k, 3395469782, f[l]), k = g, g = e, e = r(d, 30), d = a, a = q;
      h[0] = t(a, h[0]);
      h[1] = t(d, h[1]);
      h[2] = t(e, h[2]);
      h[3] = t(g, h[3]);
      h[4] = t(k, h[4])
    }
    return h
  }
  "function" === typeof define && define.amd ? define(function () {
    return n
  }) : "undefined" !== typeof exports ? "undefined" !== typeof module && module.exports ? module.exports = exports = n : exports = n : E.jsSHA = n
})(this);
if (!Array.prototype.forEach) {
  Array.prototype.forEach = function (callback, thisArg) {
    var T, k;
    if (this == null) {
      throw new TypeError(" this is null or not defined");
    }
    var O = Object(this);
    var len = O.length >>> 0;
    if ({}.toString.call(callback) != "[object Function]") {
      throw new TypeError(callback + " is not a function");
    }
    if (thisArg) {
      T = thisArg;
    }
    k = 0;
    while (k < len) {
      var kValue;
      if (k in O) {
        kValue = O[k];
        callback.call(T, kValue, k, O);
      }
      k++;
    }
  };
}
if (!String.prototype.trim) {
  String.prototype.trim = function () {
    return this.replace(/^\s+|\s+$/g, '');
  };
}
if (!Function.prototype.bind) {
  Function.prototype.bind = function (oThis) {
    if (typeof this !== 'function') {
      throw new TypeError('Function.prototype.bind - what is trying to be bound is not callable');
    }
    var aArgs = Array.prototype.slice.call(arguments, 1),
      fToBind = this,
      fNOP = function () {},
      fBound = function () {
        return fToBind.apply(this instanceof fNOP && oThis ? this : oThis, aArgs.concat(Array.prototype.slice.call(arguments)));
      };
    if (this.prototype) fNOP.prototype = this.prototype;
    fBound.prototype = new fNOP();
    return fBound;
  };
}
var page = function (controller, action, fn) {
  var page = document.body.classList;
  if (controller === page[0] && action === page[1]) fn();
  return;
}
page('content', 'setting', function () {
  var i2 = itest5ch;
  i2.env.controller = 'setting';
  i2.dom.nolink();
  i2.ronin.checkStatus();
  i2.addEvent.loginRonin().logoutRonin();
  if (i2.env.browsing_mode == 'private') {
    $('#delete_subback_history').addClass('off').text('利用不可');
    $('#active_aa_font').find('.onoffswitch').addClass('setting_btn off').text('利用不可');
    $('#warning_localstorage').removeClass('hidden');
  }
  var use_l50 = itest5ch.db.get('use_l50');
  if (use_l50 == null) use_l50 = false;
  $('#use_l50').prop('checked', use_l50 == 'true' ? true : false);
  $('#use_l50').bind('click', function () {
    if (itest5ch.env.use_localstrage() == false) {
      alert('ローカルストレージが使えない状態では利用できないオプションです。');
      return false;
    } else {
      var checked = $('#use_l50').prop('checked');
      itest5ch.db.set('use_l50', checked);
    }
  });
  var use_treeview = itest5ch.db.get('use_treeview');
  if (use_treeview == null) use_treeview = false;
  $('#use_treeview').prop('checked', use_treeview == 'true' ? true : false);
  $('#use_treeview').bind('click', function () {
    if (itest5ch.env.use_localstrage() == false) {
      alert('ローカルストレージが使えない状態では利用できないオプションです。');
      return false;
    } else {
      var checked = $('#use_treeview').prop('checked');
      itest5ch.db.set('use_treeview', checked);
    }
  });
  var TextarFont = itest5ch.db.get('TextarFont');
  $('#use_thread_aa_font').prop('checked', (TextarFont === null) ? false : true);
  $('#always_pc_view').prop('checked', ($.cookie('viewer') === 'pc') ? true : false);
  $('#use_thread_aa_font').bind('click', function () {
    var checked = !$('#use_thread_aa_font').prop('checked');
    $('#use_thread_aa_font').prop('checked', checked);
    var TextarFont = itest5ch.db.get('TextarFont');
    if (TextarFont === null) {
      if (window.confirm('AA表示フォントをダウンロードします。ダウンロードには時間がかかる可能性があります。よろしいですか?容量は1.2MBです。')) {
        itest5ch.Loading.show();
        $.ajax({
          type: 'GET',
          url: '/assets/font/textar-base64.txt',
          success: function (response) {
            try {
              itest5ch.db.set('TextarFont', response);
              alert('完了しました。AAをお楽しみ下さい。');
              $('#use_thread_aa_font').prop('checked', !checked);
            } catch (e) {
              alert('ローカルストレージが使えない状態では利用できないオプションです。');
            }
            itest5ch.Loading.hide();
          },
          error: function (xhr, type) {
            alert('ダウンロードに失敗しました。');
            $('#loading_wrap').addClass('hidden');
          }
        });
      }
    } else {
      if (window.confirm('AA表示フォントを削除しようとしています。よろしいですか?')) {
        window.localStorage.removeItem('TextarFont');
        alert('削除しました');
        $('#use_thread_aa_font').prop('checked', !checked);
      } else {
        document.getElementById('use_thread_aa_font').checked = true;
      }
    }
  });
  $('#backPage').bind('click', function () {
    if (!!document.referrer) {
      history.back();
    } else {
      window.location.href = window.location.origin;
    }
  });
  $('#delete_subback_history').bind('click', function (e) {
    var $delete_subback_history = $('#delete_subback_history');
    if ($delete_subback_history.hasClass('off')) return;
    if (window.confirm('閲覧履歴を削除しようとしています。よろしいですか?')) {
      itest5ch.db.init();
      var db = itest5ch.db.load();
      db.truncate("subbacks");
      db.commit();
      $delete_subback_history.addClass('off');
      $delete_subback_history.unbind('click');
    }
  });
  $('#delete_thread_history').bind('click', function (e) {
    if (window.confirm('閲覧履歴を削除しようとしています。よろしいですか?')) {
      $.each(document.cookie.split(';'), function (k, v) {
        var i = v.indexOf('=');
        var n = i > -1 ? v.substr(0, i) : v;
        n = n.replace(/(^\s+)|(\s+$)/g, '');
        document.cookie = n + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
      });
      itest5ch.db.init();
      var db = itest5ch.db.load();
      db.truncate("threads");
      db.commit();
      var $delete_thread_history = $('#delete_thread_history');
      $delete_thread_history.addClass('off');
      $delete_thread_history.unbind('click');
    }
  });
  $('#always_pc_view').click(function (e) {
    if ($(this).prop('checked') === true) {
      $.cookie('viewer', 'pc', {
        expires: 3,
        path: '/'
      });
    } else {
      $.cookie('viewer', 'smartphone', {
        expires: 3,
        path: '/'
      });
    }
  });
});
page('content', 'help', function () {});
page('content', 'index', function () {
  var i2 = itest5ch;
  var addEvent = i2.Bbsmenu.addEvent;
  i2.db.init();
  i2.dom.nolink();
  i2.Bbsmenu.yokumiru().contentAds().thread_histories(false);
  i2.addEvent.stop_submit(function () {
    $('#search_board').trigger('click');
  });
  addEvent.searchThread().searchBoard().showBoards().closeBoards().toggleCategory();
});
page('content', 'topics', function () {
  var i2 = itest5ch;
  var addEvent = i2.Bbsmenu.addEvent;
  i2.Bbsmenu.contentAds().yokumiru();
  addEvent.searchThread().searchBoard().showBoards().closeBoards().toggleCategory();
  i2.addEvent.stop_submit(function () {
    $('#search_board').trigger('click');
  });
});
page('content', 'find', function () {
  var i2 = itest5ch;
  var addEvent = i2.Bbsmenu.addEvent;
  i2.Bbsmenu.yokumiru();
  addEvent.searchThread().searchBoard().showBoards().closeBoards().toggleCategory();
  i2.addEvent.stop_submit(function () {
    $('#search_board').trigger('click');
  });
  i2.Search.findPage(function () {
    $('.js-render-after').removeClass('hidden');
    i2.dom.addIframe(document.querySelector('#js-adstir-320x50'), i2.Ad.tags.find.top_320x50, 320, 50);
    i2.dom.addIframe(document.querySelector('#js-adstir-300x250'), i2.Ad.tags.find.bottom_300x250, 300, 250, "padding-10px");
    $('.js-ad_iframe_wrap').each(function (k, v) {
      if ($(v).is('.js-third,.js-infeed')) $(v).removeClass('js-third js-infeed');
      if ($(v).is('.js-movie,.js-subback-native')) $(v).closest('li.ad').remove();
    });
    i2.Ad.network();
  });
});
page('content', 'histories', function () {
  var i2 = itest5ch;
  var addEvent = i2.Bbsmenu.addEvent;
  i2.Bbsmenu.thread_histories(true);
  i2.Bbsmenu.yokumiru();
  addEvent.searchThread().searchBoard().showBoards().closeBoards().toggleCategory();
  i2.addEvent.stop_submit(function () {
    $('#search_board').trigger('click');
  });
  i2.dom.addIframe(document.querySelector('#js-adstir-320x50'), i2.Ad.tags.histories.top_320x50, 320, 50, "padding-bottom-10px");
  i2.dom.addIframe(document.querySelector('#js-adstir-300x250'), i2.Ad.tags.histories.bottom_300x250, 300, 250, "padding-10px");
});
page('subback', 'show', function () {
  var i2 = itest5ch;
  i2.db.init();
  i2.env.controller = 'subback';
  i2.request.board = i2.Subback.getBoardNameByURL();
  i2.dom.nolink();
  i2.Loading.show();
  i2.Subback.show(function () {
    i2.Loading.hide();
    $('#js-forms').removeClass('hidden');
    i2.Subback.create_or_update();
    setTimeout(function () {
      i2.Ad.network();
      i2.dom.addIframe(document.querySelector('#js-adstir-300x250'), i2.Ad.tags.subback.bottom_300x250, 300, 250, "padding-10px");
    }, 100);
  });
  i2.addEvent.viewerSetting().loginRonin().logoutRonin().UseRoninCheckbox().messageHelper().toggle_header('#subback').stop_submit(function () {
    $('#search_board').trigger('click');
  });
  i2.Subback.addEvent.reload().sort();
  i2.Bbsmenu.yokumiru();
  i2.Bbsmenu.addEvent.searchThread().searchBoard().showBoards(function () {
    $('.header').removeClass('header-absolute');
  }).closeBoards(function () {
    $('.header').addClass('header-absolute');
  }).toggleCategory();
  i2.Subback.addEvent.choiceArea().moreSubback().changeCharset().click_blackbox().floatNav().goForm().go_bottom().go_top();
});
page('thread', 'show', function () {
  var i2 = itest5ch;
  var addEvent = i2.Thread.addEvent;
  i2.Thread.initialize();
  i2.Thread.download_and_render(function () {
    i2.dom.nolink();
    i2.Thread.showNewResponseButton();
    setTimeout(function () {
      i2.Thread.loadSproutAd();
      i2.Ad.network();
      console.log(i2.Ad.tags.thread["header_320x50_" + i2.number.random(2)]);
      i2.dom.addIframe(document.querySelector('#js-header-320x50'), i2.Ad.tags.thread["header_320x50_" + i2.number.random(2)], 320, 50);
    }, 100);
  });
  i2.addEvent.viewerSetting().messageHelper().loginRonin().logoutRonin().UseRoninCheckbox().go_writeform().closeWriteFormWindow().gesture_to_move('thread').clickEventTracker('thread');
  addEvent.showReply().showReplyLink().changeFontSize().moveNewResponse().go_top().popup_thumbnail().click_blackbox().jumpPage().floatNav().changeCharset().show_response().searchResponse().showCommentsByID();
  i2.Subback.addEvent.moreSubback();
  addEvent.click_closebox().more_res().popup_res().close_comment_window().toggleImageViewTools();
  $('#search_form').addClass('scroll');
  addEvent.loadSubbackOnBottom().go_bottom();
});

