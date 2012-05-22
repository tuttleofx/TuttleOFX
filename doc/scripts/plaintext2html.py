#!/usr/bin/env python
from __future__ import with_statement

import re
import cgi

colorcodes =   {'bold':{True:'\033[1m',False:'\033[22m'},
                'cyan':{True:'\033[1;36m',False:'\033[0;0m'},
                '#8E4429':{True:'\033[1;33m',False:'\033[0;0m'},
                '#0000B0':{True:'\033[1;34m',False:'\033[0;0m'},
                '#B63A11':{True:'\033[1;31m',False:'\033[0;0m'},
                'magenta':{True:'\033[1;35m',False:'\033[0;0m'},
                'green':{True:'\033[1;32m',False:'\033[0;0m'},
                'underline':{True:'\033[1;4m',False:'\033[0;0m'}}

def recolor(color, text):
    regexp = "(?:%s)(.*?)(?:%s)" % (colorcodes[color][True], colorcodes[color][False])
    regexp = regexp.replace('[', r'\[')
    return re.sub(regexp, r'''<span style="color: %s">\1</span>''' % color, text)

def resinglecolor(color, text, intxt):
    regexp = "(?:\033\[1;32m%s)(.*?)" % intxt
    return re.sub(regexp, r'<span style="color: green">%s\1</span>'% intxt, text)

def removestdcolor(text):
    regexp = "(?:\033\[0;0m)(.*?)"
    return re.sub(regexp, r'', text)

def bold(text):
    regexp = "(?:%s)(.*?)(?:%s)" % (colorcodes['bold'][True], colorcodes['bold'][False])
    regexp = regexp.replace('[', r'\[')
    return re.sub(regexp, r'<span style="font-weight:bold">\1</span>', text)

def underline(text):
    regexp = "(?:%s)(.*?)(?:%s)" % (colorcodes['underline'][True], colorcodes['underline'][False])
    regexp = regexp.replace('[', r'\[')
    return re.sub(regexp, r'<span style="text-decoration: underline">\1</span>', text)

def removebells(text):
    return text.replace('\07', '')

def removebackspaces(text):
    backspace_or_eol = r'(.\010)|(\033\[K)'
    n = 1
    while n > 0:
        text, n = re.subn(backspace_or_eol, '', text, 1)
    return text

template = '''\
<html>
<head>

</head>
<body>
%s
</body>
</html>
'''

re_string = re.compile(r'(?P<htmlchars>[<&>])|(?P<space>^[ \t]+)|(?P<lineend>\r\n|\r|\n)|(?P<protocal>(^|\s|\[)((http|ftp)://.*?))(\s|$|\])', re.S|re.M|re.I)
def plaintext2html(text, tabstop=4):
    def do_sub(m):
        c = m.groupdict()
        if c['htmlchars']:
            return cgi.escape(c['htmlchars'])
        if c['lineend']:
            return '<br>'
        elif c['space']:
            t = m.group().replace('\t', '&nbsp;'*tabstop)
            t = t.replace(' ', '&nbsp;')
            return t
        elif c['space'] == '\t':
            return ' '*tabstop;
        else:
            url = m.group('protocal')
            #print url
            if url.startswith('['):
                prefix = '['
                suffix = ']'
                url = url[1:]
            else:
                prefix = ''
                suffix = ''
            last = m.groups()[-1]
            if last in ['\n', '\r', '\r\n']:
                last = '<br>'
            return '%s<a href=%s>%s</a>%s' % (prefix, url, url, suffix)
    result = re.sub(re_string, do_sub, text)
    result = recolor('cyan', result)
    result = recolor('#8E4429', result)
    result = recolor('#0000B0', result)
    result = recolor('#B63A11', result)
    result = recolor('magenta', result)
    result = recolor('green', result)

    result = resinglecolor('green', result, 'Source')
    result = resinglecolor('green', result, 'Output')

    result = bold(result)
    result = underline(result)
    result = removebells(result)
    result = removebackspaces(result)

    result = removestdcolor(result)

    return template % result


if __name__ == '__main__':
    import sys
    with open(sys.argv[-1]) as f:
        text = f.read()
    print plaintext2html(text)
