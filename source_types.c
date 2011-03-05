//// source_types.
//
// source type determination.
//
// Bubbs, July/2010.


#if defined(TIMEWARP)
#include <strings.h>

#else
#include <sys/strings.h>

#endif


/// source_types definitions.

// source_types index and width definitions.
#define SRCTYP_DESCRIPTION                0     // string   description of source type.
#define SRCTYP_FILE_EXTENSIONS            1     // string*  matching file extensions.
#define SRCTYP_MATCH_REGEXP               2     // string   optional matching regular expression.

#define SOURCE_TYPES_WIDTH                (SRCTYP_MATCH_REGEXP +1)

// Default source type.
#define DEFAULT_SOURCE_TYPE               "text/plain"

// Default source types file.
#if !defined(DEFAULT_SOURCE_TYPES_FILE)
#define DEFAULT_SOURCE_TYPES_FILE         "/" __DIR__ "source_types.ini"
#endif


/// source_types variables.

nosave    mapping source_types          = 0;


/// source_types query method prototypes.

public    string* query_source_types      ();
public    string  query_source_type_desc  (string source_type);
public    string* query_source_type_exts  (string source_type);
public    string  query_source_type_regexp(string source_type);


/// source_types process method prototypes.

public    mapping read_source_types       (string data_file);
public    string  determine_source_type   (string url, string source);
public    string  unquote_string          (string str);


/// source_types mudlib interface methods.

public create() {
  source_types = read_source_types(DEFAULT_SOURCE_TYPES_FILE);
}

public int deny_update() { return 0; }
public int clean_up(int arg) {
  if (deny_update())
    return 1;
  destruct(this_object());
  return 0;
}

public void destructor() { }


/// source_types query methods.

public string* query_source_types() {
  return
    mappingp(source_types) &&
    m_indices(source_types);
}

public string query_source_type_desc(string source_type) {
  return
    mappingp(source_types) &&
    stringp(source_types[source_type, SRCTYP_DESCRIPTION]) &&
    strlen(source_types[source_type, SRCTYP_DESCRIPTION]) &&
    source_types[source_type, SRCTYP_DESCRIPTION];
}

public string* query_source_type_exts(string source_type) {
  return
    mappingp(source_types) &&
    pointerp(source_types[source_type, SRCTYP_FILE_EXTENSIONS]) &&
    sizeof(source_types[source_type, SRCTYP_FILE_EXTENSIONS]) &&
    source_types[source_type, SRCTYP_FILE_EXTENSIONS];
}

public string query_source_type_regexp(string source_type) {
  return
    mappingp(source_types) &&
    stringp(source_types[source_type, SRCTYP_MATCH_REGEXP]) &&
    strlen(source_types[source_type, SRCTYP_MATCH_REGEXP]) &&
    source_types[source_type, SRCTYP_MATCH_REGEXP];
}


/// source_types process methgods.

public mapping read_source_types(string data_file) {
  string  file_data   = stringp(data_file) && read_file(data_file);
  if (!file_data)
    return 0;

  mapping file_stypes = ([:SOURCE_TYPES_WIDTH ]);
  foreach(string line : explode(file_data, "\n"))
    if (strlen(line) && strstr(line, ";") && strstr(line, "#")) {
      int     pos   = member(line, '=');
      string  stype = (pos != -1) && line[..pos -1];
      string* bits  = (pos != -1) && pos && explode(line[pos +1..], ":");
      pos = sizeof(bits);
//printf("stype=%#Q; sizeof(bits)=%#Q; bits=%#Q\n", stype, pos, bits);
      if (line && (pos > SRCTYP_DESCRIPTION) && strlen(bits[SRCTYP_DESCRIPTION])) {
        m_add(file_stypes, stype);
        file_stypes[stype, SRCTYP_DESCRIPTION]     = bits[SRCTYP_DESCRIPTION];
        file_stypes[stype, SRCTYP_FILE_EXTENSIONS] =
          (pos > SRCTYP_FILE_EXTENSIONS) &&
          filter(map(explode(bits[SRCTYP_FILE_EXTENSIONS], ","), #'trim, TRIM_BOTH, " \t."), #'strlen);
        file_stypes[stype, SRCTYP_MATCH_REGEXP]    =
          (pos > SRCTYP_MATCH_REGEXP) &&
          unquote_string(implode(bits[SRCTYP_MATCH_REGEXP..], ":"));
//printf("%#Q\n", file_stypes & ({ stype, }));
     }
    }

  return sizeof(file_stypes) ? file_stypes : (source_types || ([:SOURCE_TYPES_WIDTH ]));
}

public string determine_source_type(string url, string source) {
  string source_type = 0;

  if (stringp(url) && source_types) {
    int    pos  = rmember(url, '/');
    string name = (pos == -1) ? url : url[pos +1..];
    pos = rmember(name, '.');
    string ext  = (pos == -1) ? 0   : name[pos +1..];
    int    len  = stringp(source) && strlen(source);

    if (ext)
      foreach(string stype : source_types)
        source_type ||=
          ((source_types[stype, SRCTYP_FILE_EXTENSIONS] && (member(source_types[stype, SRCTYP_FILE_EXTENSIONS], ext) != -1)) ||
           (len && source_types[source_type, SRCTYP_MATCH_REGEXP] && (strlen(regreplace(source, source_types[source_type, SRCTYP_MATCH_REGEXP], "", 0)) != len))) &&
          stype;

    if (!source_type)
      foreach(string stype : source_types)
        source_type ||=
          source_types[stype, SRCTYP_FILE_EXTENSIONS] && (member(source_types[stype, SRCTYP_FILE_EXTENSIONS], "*") != -1) && stype;
  }

  return source_type || DEFAULT_SOURCE_TYPE;
}

public string unquote_string(string str) {
  string output = "";
  int    length = strlen(str);
  for(int index = 0; index < length; index++) {
    if (str[index] == '\\')
      switch((++index < length) && str[index]) {
        case 0  :                                break;
        case 'a':  output += "\a";               break;
        case 'b':  output += "\b";               break;
        case 'e':  output += "\e";               break;
        case 'f':  output += "\f";               break;
        case 'n':  output += "\n";               break;
        case 'r':  output += "\r";               break;
        case 't':  output += "\t";               break;
        case '0'..'9':
          string numeric = regreplace(str[index..], "^([0-9](([0-9]([0-9]|))|)).*$", "\\1", 0);
          output += sprintf("%c", to_int(numeric));
          index  += strlen(numeric) -1;
        break;
        default :  output += str[index..index];  break;
      }
    else
      output += str[index..index];
  }
  return output;
}

