import re
funcs = '''
int db_{0}_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const char* secondary , size_t len);
void db_{0}_update( int iterator, uint64_t payer, const char* secondary , size_t len );
void db_{0}_remove( int iterator );
int db_{0}_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const char* secondary , size_t len, uint64_t* primary );
int db_{0}_find_primary( uint64_t code, uint64_t scope, uint64_t table, const char* secondary , size_t len, uint64_t primary );
int db_{0}_lowerbound( uint64_t code, uint64_t scope, uint64_t table,  const char* secondary , size_t len, uint64_t* primary );
int db_{0}_upperbound( uint64_t code, uint64_t scope, uint64_t table,  const char* secondary , size_t len, uint64_t* primary );
int db_{0}_end( uint64_t code, uint64_t scope, uint64_t table );
int db_{0}_next( int iterator, uint64_t* primary  );
int db_{0}_previous( int iterator, uint64_t* primary );
int db_{0}_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const char* data, size_t data_len);
void db_{0}_update( int iterator, uint64_t payer, const char* data, size_t data_len );
void db_{0}_remove( int iterator );
int db_{0}_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const char* data, size_t data_len, uint64_t* primary );
int db_{0}_find_primary( uint64_t code, uint64_t scope, uint64_t table, char* data, size_t data_len, uint64_t primary );
int db_{0}_lowerbound( uint64_t code, uint64_t scope, uint64_t table, char* data, size_t data_len, uint64_t* primary );
int db_{0}_upperbound( uint64_t code, uint64_t scope, uint64_t table, char* data, size_t data_len, uint64_t* primary );
int db_{0}_end( uint64_t code, uint64_t scope, uint64_t table );
int db_{0}_next( int iterator, uint64_t* primary  );
int db_{0}_previous( int iterator, uint64_t* primary );
'''
s = "int   db_{0}_store(    uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const char* secondary , size_t len);"

s = "int db_{0}_next( int iterator, uint64_t* primary  );"

def filter_space(s):
    ss = []
    for item in s.split(' '):
        item = item.strip()
        if item:
            ss.append(item)

    return ' '.join(ss)

s = filter_space(s)
print(s)

def parse(func, idx=None):
    start = func.find('(')
    if start < 0:
        return
    ret_type, func_name = func[:start].split(' ')
    print(ret_type, func_name)
    if idx:
        func_name = func_name.format(idx)

    end = func.rfind(')')
    
    params = []
    for _s in func[start+1:end].split(','):
        pos = _s.strip().rfind(' ')
        params.append([_s[:pos+1].strip(), _s[pos+1:].strip()])
    print(params)
    
    defines = []
    i = 0
    codes = []
    arguments = []
    while True:
        param = params[i]
        if param[0] == 'uint64_t':
            code = 'uint64_t _{0} = mp_obj_get_uint(args[{1}]);'.format(param[1], i)
            codes.append(code)
            arguments.append('_{0}'.format(param[1]))
        elif param[0] == 'const char*':
            defines.append('size_t _{0}_len'.format(param[1]))
            code = 'const char* _{0} = (void *)mp_obj_str_get_data(args[{1}], &_{2}_len);'.format(param[1], i, param[1])
            codes.append(code)
            arguments.append('_{0}'.format(param[1]))
            arguments.append('_{0}_len'.format(param[1]))
            i += 1 #skip length parameter
        elif param[0] = 'int':
            pass
        
        i += 1
        if i >= len(params):
            break
    returns = ''
    if ret_type == 'int':
        returns = 'int ret = '
    lines = []
    
    result = defines + codes
    lines += result

    if ret_type == 'int':
        lines.append('return mp_obj_new_int(ret);')
    elif ret_type == 'void':
        lines.append('return mp_const_none;')

    func_call = returns + func_name + '(' + ', '.join(arguments) + ')'
    lines.append(func_call)

    for line in lines:
        print('    ', line)
    

parse(s, 'idx128')

if 0:
    for func in funcs.split('\n'):
        parse(func, 'idx128')

'''
prog = re.compile(r'(\w+)\s+([\w#\s]+)(\()([^,]+),([^,]+),([^,]+),{0,}')
result = prog.search(s)
print(result.groups())

m = re.match(r"(\w+) (\w+)", "Isaac Newton, physicist")
print(m.groups())

for func in funcs.split('\n'):
    break
    print(func.strip())
'''
