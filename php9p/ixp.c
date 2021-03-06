/*
   +-----------------------------------------------------------------------+
   | Permission is hereby granted, free of charge, to any person obtaining |
   | a copy of this software and associated documentation files (the       |
   | "Software"), to deal in the Software without restriction, including   |
   | without limitation the rights to use, copy, modify, merge, publish,   |
   | distribute, sublicense, and/or sell copies of the Software, and to    |
   | permit persons to whom the Software is furnished to do so, subject to |
   | the following conditions:                                             |
   |                                                                       |
   | The above copyright notice and this permission notice shall be        |
   | included in all copies or substantial portions of the Software.	   |
   |                                                                       |
   | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	   |
   | EXPRESS OR	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    |
   | MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.|
   | IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  |
   | CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  |
   | TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     |
   | SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                |
   +-----------------------------------------------------------------------+
   | Authors: Anant Narayanan <anant@php.net>                              |
   +-----------------------------------------------------------------------+
*/

/* $ Id: $ */ 

#include "php_ixp.h"

#if HAVE_IXP

/* {{{ Class entries */
static zend_class_entry * IxpCallbacks_ce_ptr = NULL;
static zend_class_entry * IxpServerCallbacks_ce_ptr = NULL;
static zend_class_entry * IxpClient_ce_ptr = NULL;
static zend_class_entry * IxpCFid_ce_ptr = NULL;
static zend_class_entry * IxpStat_ce_ptr = NULL;
static zend_class_entry * IxpQid_ce_ptr = NULL;
static zend_class_entry * IxpConn_ce_ptr = NULL;
static zend_class_entry * IxpFid_ce_ptr = NULL;
static zend_class_entry * IxpRequest_ce_ptr = NULL;
static zend_class_entry * IxpFcall_ce_ptr = NULL;
static zend_class_entry * IxpServer_ce_ptr = NULL;
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(ixp)
{
	class_init_IxpClient();
	class_init_IxpCFid();
	class_init_IxpStat();
	class_init_IxpQid();
	class_init_IxpConn();
	class_init_IxpFid();
	class_init_IxpRequest();
	class_init_IxpFcall();
	class_init_IxpServer();
	interface_init_IxpCallbacks();
	interface_init_IxpServerCallbacks();

	REGISTER_STRING_CONSTANT("IXP_VERSION", IXP_VERSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IXP_NOTAG", IXP_NOTAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IXP_NOFID", IXP_NOFID, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */


/* {{{ Helper functions */
static zval * object_instantiate(zend_class_entry *ce, zval *object TSRMLS_DC)
{
	if (!object)
		ALLOC_ZVAL(object);

	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, ce);
	object->refcount = 1;
	object->is_ref = 0;
	return object;
}

static void PHP_IxpQid_initialize(zval *obj, IxpQid from)
{
	zend_class_entry *_this_ce = Z_OBJCE_P(obj);
	zval *_this_zval = obj;

	PHP_IxpQid *qid = FETCH_IxpQid(_this_zval);
	qid->ptr = &from;

	PROP_SET_LONG(type, from.type);
	PROP_SET_LONG(version, from.version);
	PROP_SET_LONG(path, from.path);
}

static void PHP_IxpCFid_initialize(zval *obj, IxpCFid *from)
{
	zval *ret;
	zend_class_entry *_this_ce = Z_OBJCE_P(obj);
	zval *_this_zval = obj;

	PHP_IxpCFid *cfid = FETCH_IxpCFid(_this_zval);
	cfid->ptr = from;

	PROP_SET_LONG(fid, from->fid);
	PROP_SET_DOUBLE(mode, from->mode);
	PROP_SET_LONG(open, from->open);
	PROP_SET_LONG(iounit, from->iounit);
	PROP_SET_LONG(offset, from->iounit);
}

static void PHP_IxpStat_initialize(zval *obj, IxpStat *from)
{
	zval *ret;
	zend_class_entry *_this_ce = Z_OBJCE_P(obj);
	zval *_this_zval = obj;

	PHP_IxpStat *stat = FETCH_IxpStat(_this_zval);
	stat->ptr = from;
	
	PROP_SET_LONG(type, from->type);
	PROP_SET_LONG(device, from->dev);
	PROP_SET_DOUBLE(mode, from->mode);
	PROP_SET_LONG(aTime, from->atime);
	PROP_SET_LONG(mTime, from->length);
	PROP_SET_STRING(name, from->name);
	PROP_SET_STRING(uid, from->uid);
	PROP_SET_STRING(gid, from->gid);
	PROP_SET_STRING(muid, from->muid);
}

static void PHP_IxpConn_initialize(zval *obj, IxpConn *from)
{
	zend_class_entry *_this_ce = Z_OBJCE_P(obj);
	zval *_this_zval = obj;

	PHP_IxpConn *qid = FETCH_IxpConn(_this_zval);
	qid->ptr = from;

	PROP_SET_LONG(closed, from->closed);
}

static void PHP_IxpRequest_initialize(zval *obj, Ixp9Req *from)
{
	zend_class_entry *_this_ce = Z_OBJCE_P(obj);
	zval *_this_zval = obj;

	PHP_IxpRequest *req = FETCH_IxpRequest(_this_zval);
	req->ptr = from;
}

static void PHP_IxpFid_initialize(zval *obj, IxpFid *from)
{
	zval *ret;
	zend_class_entry *_this_ce = Z_OBJCE_P(obj);
	zval *_this_zval = obj;

	PHP_IxpFid *fid = FETCH_IxpFid(_this_zval);
	fid->ptr = from;

	PROP_SET_STRING(uid, from->uid);
	PROP_SET_LONG(fid, from->fid);
	PROP_SET_DOUBLE(omode, from->omode);
}

static void PHP_IxpFcall_initialize(zval *obj, IxpFcall *from)
{
	zval *ret;
	zend_class_entry *_this_ce = Z_OBJCE_P(obj);
	zval *_this_zval = obj;

	PHP_IxpFcall *fcall = FETCH_IxpFcall(_this_zval);
	fcall->ptr = from;

	PROP_SET_LONG(type, from->type);
	PROP_SET_LONG(tag, from->tag);
	PROP_SET_LONG(fid, from->fid);
	PROP_SET_LONG(msize, from->msize);
	PROP_SET_STRING(version, from->version);
	PROP_SET_LONG(oldtag, from->oldtag);
	PROP_SET_STRING(ename, from->ename);
	PROP_SET_LONG(iounit, from->iounit);
	PROP_SET_LONG(afid, from->afid);
	PROP_SET_STRING(uname, from->uname);
	PROP_SET_STRING(aname, from->aname);
	PROP_SET_LONG(perm, from->perm);
	PROP_SET_STRING(name,from->name);
	PROP_SET_LONG(mode, from->mode);
	PROP_SET_LONG(newfid, from->newfid);
	PROP_SET_LONG(nwname, from->nwname);
	PROP_SET_STRING(wname, (char *)from->wname);
	PROP_SET_LONG(nwqid, from->nwqid);
	PROP_SET_DOUBLE(offset, from->offset);
	PROP_SET_LONG(count, from->count);
	PROP_SET_STRING(data, from->data);
	PROP_SET_LONG(nstat, from->nstat);
	PROP_SET_STRING(stat, (char *)from->stat);
}


/* }}} helper functions */


/* {{{ Marshallers */
static void PHP_IxpServerCallbacks_read_marshal(IxpConn *conn)
{
	IxpServer *srv;
	srv = conn->srv;
	if (srv->aux == NULL) {
		serve_9pcon(conn);
	} else {
		zval *cb, *ret, *args[1], retval, funcname;
		ZVAL_STRING(&funcname, "read", 0);
		cb = srv->aux;
		object_instantiate(IxpConn_ce_ptr, ret);
		PHP_IxpConn_initialize(ret, conn);
		args[0] = ret;
		call_user_function(EG(function_table), &cb, &funcname, &retval, 1, args TSRMLS_CC);
	}
}

static void PHP_IxpServerCallbacks_close_marshal(IxpConn *conn)
{
	IxpServer *srv;
	srv = conn->srv;
	if (srv->aux != NULL) {
		zval *cb, *ret, *args[1], retval, funcname;
		ZVAL_STRING(&funcname, "close", 0);
		cb = srv->aux;
		object_instantiate(IxpConn_ce_ptr, ret);
		PHP_IxpConn_initialize(ret, conn);
		args[0] = ret;
		call_user_function(EG(function_table), &cb, &funcname, &retval, 1, args TSRMLS_CC);
	}
}

static void PHP_IxpCallbacks_attach_marshal(Ixp9Req *req)
{
	Ixp9Srv *srv;
	srv = req->srv;

	zval *cb, *ret, *args[1], retval, funcname;
	ZVAL_STRING(&funcname, "attach", 0);
	cb = srv->aux;	
}

static void PHP_IxpCallbacks_clunk_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_create_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_flush_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_open_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_read_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_remove_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_stat_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_walk_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_write_marshal(Ixp9Req *req)
{

}

static void PHP_IxpCallbacks_freefid_marshal(IxpFid *fid)
{

}

/* }}} Marshallers */


/* {{{ Interface IxpCallbacks */
static zend_function_entry IxpCallbacks_methods[] = {
	ZEND_FENTRY(attach, NULL, IxpCallbacks__attach_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(clunk, NULL, IxpCallbacks__clunk_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(create, NULL, IxpCallbacks__create_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(flush, NULL, IxpCallbacks__flush_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(open, NULL, IxpCallbacks__open_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(read, NULL, IxpCallbacks__read_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(remove, NULL, IxpCallbacks__remove_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(stat, NULL, IxpCallbacks__stat_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(walk, NULL, IxpCallbacks__walk_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(write, NULL, IxpCallbacks__write_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(freefid, NULL, IxpCallbacks__freefid_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void interface_init_IxpCallbacks(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpCallbacks", IxpCallbacks_methods);
	IxpCallbacks_ce_ptr = zend_register_internal_interface(&ce TSRMLS_CC);
}
/* }}} Interface IxpCallbacks */

/* {{{ Interface IxpServerCallbacks */
static zend_function_entry IxpServerCallbacks_methods[] = {
	ZEND_FENTRY(read, NULL, IxpServerCallbacks__read_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	ZEND_FENTRY(close, NULL, IxpServerCallbacks__close_args, ZEND_ACC_ABSTRACT |  ZEND_ACC_INTERFACE | ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void interface_init_IxpServerCallbacks(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpServerCallbacks", IxpServerCallbacks_methods);
	IxpServerCallbacks_ce_ptr = zend_register_internal_interface(&ce TSRMLS_CC);
}
/* }}} Interface IxpServerCallbacks */


/* {{{ Class IxpClient */

static zend_object_handlers PHP_IxpClient_handlers;

static void PHP_IxpClient_object_free(void *object TSRMLS_DC)
{
	PHP_IxpClient *intern = (PHP_IxpClient *)object;

	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);
}

static zend_object_value PHP_IxpClient_object_new_ex(zend_class_entry *class_type, PHP_IxpClient **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpClient *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpClient));
	memset(intern, 0, sizeof(PHP_IxpClient));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpClient_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpClient_handlers;
	return retval;
}

static zend_object_value PHP_IxpClient_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpClient *tmp;
	return PHP_IxpClient_object_new_ex(class_type, &tmp TSRMLS_DC);
}

/* {{{ proto void construct(string path) */
PHP_METHOD(IxpClient, __construct)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	IxpClient *client;

	char * path = NULL;
	int path_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) { 
		return;
	}

	client = ixp_mount(path);
	if (client == nil) {
		php_error(E_WARNING, ixp_errbuf());
		RETURN_FALSE;
	}

	_this_zval = getThis();
    _this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpClient *object = FETCH_IxpClient(_this_zval);

	object->ptr = client;
	PROP_SET_LONG(mSize, client->msize);
	PROP_SET_LONG(lastFid, client->lastfid);
}
/* }}} __construct */

/* {{{ proto void unmount() */
PHP_METHOD(IxpClient, unmount)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpClient_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);	
	PHP_IxpClient *object = FETCH_IxpClient(_this_zval);	
	
	ixp_unmount(object->ptr);
}
/* }}} unmount */

/* {{{ proto object create(string name [, string permissions [, string mode]]) */
PHP_METHOD(IxpClient, create)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	IxpCFid *this_cfid;

	char * name = NULL;
	int name_len = 0;
	char * permissions = NULL;
	int permissions_len = 0;
	char * mode = NULL;
	int mode_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|ss", &_this_zval, IxpClient_ce_ptr, &name, &name_len, &permissions, &permissions_len, &mode, &mode_len) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpClient *object = FETCH_IxpClient(_this_zval);
	
	this_cfid = ixp_create(object->ptr, name, 0, 0);
	object_instantiate(IxpCFid_ce_ptr, return_value TSRMLS_CC);
	PHP_IxpCFid_initialize(return_value, this_cfid);
}
/* }}} create */

/* {{{ proto object open(string name [, int mode]) */
PHP_METHOD(IxpClient, open)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	IxpCFid *this_cfid;

	char * name = NULL;
	int name_len = 0;
	int mode = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|l", &_this_zval, IxpClient_ce_ptr, &name, &name_len, &mode) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpClient *object = FETCH_IxpClient(_this_zval);

	if (mode == 0)
		mode = P9_OREAD;
	this_cfid = ixp_open(object->ptr, name, (uchar)mode);
	object_instantiate(IxpCFid_ce_ptr, return_value TSRMLS_CC);
	PHP_IxpCFid_initialize(return_value, this_cfid);
}
/* }}} open */

/* {{{ proto bool remove(string path) */
PHP_METHOD(IxpClient, remove)
{
	char * path = NULL;
	int path_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) {
		return;
	}

	PHP_IxpClient *object = FETCH_IxpClient(this_ptr);

	if (ixp_remove(object->ptr, path) == 0) {
		php_error(E_WARNING, ixp_errbuf());
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} remove */

/* {{{ proto object stat(string path) */
PHP_METHOD(IxpClient, stat)
{
	IxpStat *stat;
	char * path = NULL;
	int path_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) {
		return;
	}

	PHP_IxpClient *object = FETCH_IxpClient(this_ptr);

	stat = ixp_stat(object->ptr, path);
	if (stat == nil) {
		php_error(E_WARNING, ixp_errbuf());
		RETURN_FALSE;
	}

	object_instantiate(IxpStat_ce_ptr, return_value TSRMLS_CC);
	PHP_IxpStat_initialize(return_value, stat);
}
/* }}} stat */

/* {{{ proto object statDir(string path) */
PHP_METHOD(IxpClient, statDir)
{
	IxpStat *stat;
	IxpCFid *cfid;
	IxpMsg m;
	zval *objects;

	char * path = NULL;
	uchar * buf;
	int path_len = 0, nstat, mstat, count, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) {
		return;
	}

	PHP_IxpClient *object = FETCH_IxpClient(this_ptr);

	cfid = ixp_open(object->ptr, path, P9_OREAD);
	if (cfid == nil) {
		php_error(E_WARNING, ixp_errbuf());
		RETURN_FALSE;
	}

	if (cfid->qid.type&P9_QTDIR != 0) {
		php_error(E_WARNING, "Given path %s is not a directory!", &path);
		RETURN_FALSE;
	}

	nstat = 0;
	mstat = 16;
	stat = ixp_emalloc(sizeof(*stat) * mstat);
	buf = emalloc(cfid->iounit);

	while((count = ixp_read(cfid, buf, cfid->iounit)) > 0) {
		m = ixp_message(buf, count, MsgUnpack);
		while(m.pos < m.end) {
			if(nstat == mstat) {
				mstat <<= 1;
				stat = ixp_erealloc(stat, sizeof(*stat) * mstat);
			}
			ixp_pstat(&m, &stat[nstat++]);
		}
	}

	array_init(return_value);
	objects = emalloc(sizeof(*objects) * nstat);
	for (i = 0; i < nstat; i++) {
		object_instantiate(IxpStat_ce_ptr, &objects[i] TSRMLS_CC);
		PHP_IxpStat_initialize(&objects[i], &stat[i]);
		add_next_index_zval(return_value, &objects[i]);
		ixp_freestat(&stat[i]);
	}
	
	free(stat);
	efree(buf);
}
/* }}} stat */

static zend_function_entry IxpClient_methods[] = {
	PHP_ME(IxpClient, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpClient, unmount, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpClient, create, IxpClient__create_args, ZEND_ACC_PUBLIC)
	PHP_ME(IxpClient, open, IxpClient__open_args, ZEND_ACC_PUBLIC)
	PHP_ME(IxpClient, remove, IxpClient__remove_args, ZEND_ACC_PUBLIC)
	PHP_ME(IxpClient, stat, IxpClient__stat_args, ZEND_ACC_PUBLIC)
	PHP_ME(IxpClient, statDir, IxpClient__statDir_args, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpClient(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpClient", IxpClient_methods);
	IxpClient_ce_ptr = zend_register_internal_class(&ce);
	IxpClient_ce_ptr->create_object = PHP_IxpClient_object_new;
	memcpy(&PHP_IxpClient_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpClient_handlers.clone_obj = NULL;

	zend_declare_property_long(IxpClient_ce_ptr,
			"mSize", sizeof("mSize")-1, 0,
			ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(IxpClient_ce_ptr,
			"lastFid", sizeof("lastFid")-1, 0,
			ZEND_ACC_PUBLIC TSRMLS_CC);
	
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OREAD", 5,
			P9_OREAD TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OWRITE", 6,
			P9_OWRITE TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"ORDWR", 5,
			P9_ORDWR TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OEXEC", 5,
			P9_OEXEC TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OTRUNC", 6,
			P9_OTRUNC TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OCEXEC", 6,
			P9_OCEXEC TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"ORCLOSE", 6,
			P9_ORCLOSE TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"ODIRECT", 7,
			P9_ODIRECT TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"ONONBLOCK", 9,
			P9_ONONBLOCK TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OEXCL", 5,
			P9_OEXCL TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OLOCK", 5,
			P9_OLOCK TSRMLS_CC);
	zend_declare_class_constant_long(IxpClient_ce_ptr,
			"OAPPEND", 7,
			P9_OAPPEND TSRMLS_CC);
}

/* }}} Class IxpClient */


/* {{{ Class IxpCFid */

static zend_object_handlers PHP_IxpCFid_handlers;

static void PHP_IxpCFid_object_free(void *object TSRMLS_DC)
{
	PHP_IxpCFid *intern = (PHP_IxpCFid *)object;

	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);

	//efree(object);
}

static zend_object_value PHP_IxpCFid_object_new_ex(zend_class_entry *class_type, PHP_IxpCFid **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpCFid *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpCFid));
	memset(intern, 0, sizeof(PHP_IxpCFid));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpCFid_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpCFid_handlers;
	return retval;
}

static zend_object_value PHP_IxpCFid_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpCFid *tmp;
	return PHP_IxpCFid_object_new_ex(class_type, &tmp TSRMLS_DC);
}

/* {{{ proto string read(int count[, int offset]) */
PHP_METHOD(IxpCFid, read)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	long count = 0, offset = 0;
	int recvd;
	char *buf;

	php_stream *output;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol|l", &_this_zval, IxpCFid_ce_ptr, &count, &offset) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpCFid *object = FETCH_IxpCFid(_this_zval);

	buf = ixp_emalloc(count);
	memset(buf, '\0', count);
	if (offset == 0) {
		recvd = ixp_read(object->ptr, buf, count);
	} else {
		recvd = ixp_pread(object->ptr, buf, count, offset);
	}

	if (recvd <= 0) {
		if (recvd < 0)
			php_error(E_WARNING, ixp_errbuf());
		RETURN_FALSE;
	}

	RETVAL_STRINGL(buf, recvd, 1);
	free(buf);
}
/* }}} read */

/* {{{ proto bool write(string buffer) */
PHP_METHOD(IxpCFid, write)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	char * buffer = NULL;
	int buffer_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, IxpCFid_ce_ptr, &buffer, &buffer_len) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpCFid *object = FETCH_IxpCFid(_this_zval);

	if (ixp_write(object->ptr, buffer, buffer_len) == -1) {
		php_error(E_WARNING, ixp_errbuf());
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} write */

/* {{{ proto object getQid() */
PHP_METHOD(IxpCFid, getQid)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpCFid_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpCFid *object = FETCH_IxpCFid(_this_zval);

	object_instantiate(IxpQid_ce_ptr, return_value);
	PHP_IxpQid_initialize(return_value, object->ptr->qid);
}
/* }}} getQid */

/* {{{ proto int close() */
PHP_METHOD(IxpCFid, close)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	int ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpCFid_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpCFid *object = FETCH_IxpCFid(_this_zval);

	ret = ixp_close(object->ptr);
	RETVAL_LONG(ret);
}
/* }}} close */

static zend_function_entry IxpCFid_methods[] = {
	PHP_ME(IxpCFid, read, IxpCFid__read_args, ZEND_ACC_PUBLIC)
	PHP_ME(IxpCFid, write, IxpCFid__write_args, ZEND_ACC_PUBLIC)
	PHP_ME(IxpCFid, getQid, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpCFid, close, NULL, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpCFid(void)
{
	zend_class_entry ce;
	zval *empty = NULL;

	INIT_CLASS_ENTRY(ce, "IxpCFid", IxpCFid_methods);
	IxpCFid_ce_ptr = zend_register_internal_class(&ce);
	IxpCFid_ce_ptr->create_object = PHP_IxpCFid_object_new;
	memcpy(&PHP_IxpCFid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpCFid_handlers.clone_obj = NULL;

	zend_declare_property_long(IxpCFid_ce_ptr, 
		"fid", 3, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpCFid_ce_ptr, 
		"mode", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpCFid_ce_ptr, 
		"open", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpCFid_ce_ptr, 
		"iounit", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpCFid_ce_ptr, 
		"offset", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);
}
/* }}} Class IxpCFid */


/* {{{ Class IxpStat */

static zend_object_handlers PHP_IxpStat_handlers;

static void PHP_IxpStat_object_free(void *object TSRMLS_DC)
{
	PHP_IxpStat *intern = (PHP_IxpStat *)object;
	
	ixp_freestat(intern->ptr);
	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);
}

static zend_object_value PHP_IxpStat_object_new_ex(zend_class_entry *class_type, PHP_IxpStat **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpStat *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpStat));
	memset(intern, 0, sizeof(PHP_IxpStat));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpStat_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpStat_handlers;
	return retval;
}

static zend_object_value PHP_IxpStat_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpStat *tmp;
	return PHP_IxpStat_object_new_ex(class_type, &tmp TSRMLS_DC);
}

/* {{{ proto object getQid() */
PHP_METHOD(IxpStat, getQid)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	int ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpStat_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpStat *object = FETCH_IxpStat(_this_zval);

	object_instantiate(IxpQid_ce_ptr, return_value);
	PHP_IxpQid_initialize(return_value, object->ptr->qid);
}
/* }}} getQid */

static void
setrwx(long m, char *s) {
	static char *modes[] = {
		"---", "--x", "-w-",
		"-wx", "r--", "r-x",
		"rw-", "rwx",
	};
	strncpy(s, modes[m], 3);
}

/* {{{ proto object getMode() */
PHP_METHOD(IxpStat, getMode)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	static char buf[16];
	int ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpStat_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpStat *object = FETCH_IxpStat(_this_zval);

	buf[0] = '-';
	if (object->ptr->mode & P9_DMDIR)
		buf[0] = 'd';
	buf[1] = '-';
	setrwx((object->ptr->mode >> 6) & 7, &buf[2]);
	setrwx((object->ptr->mode >> 3) & 7, &buf[5]);
	setrwx((object->ptr->mode >> 0) & 7, &buf[8]);
	buf[11] = 0;

	RETVAL_STRING(buf, 1);
}
/* }}} getMode */

/* {{{ proto object getTime() */
PHP_METHOD(IxpStat, getTime)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	static char buf[32];
	int ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpStat_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpStat *object = FETCH_IxpStat(_this_zval);

	ctime_r((time_t*)&object->ptr->mtime, buf);
	buf[strlen(buf) - 1] = '\0';

	RETVAL_STRING(buf, 1);
}
/* }}} getTime */

static zend_function_entry IxpStat_methods[] = {
	PHP_ME(IxpStat, getQid, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpStat, getMode, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpStat, getTime, NULL, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpStat(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpStat", IxpStat_methods);
	IxpStat_ce_ptr = zend_register_internal_class(&ce);
	IxpStat_ce_ptr->create_object = PHP_IxpStat_object_new;
	memcpy(&PHP_IxpStat_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpStat_handlers.clone_obj = NULL;

	zend_declare_property_long(IxpStat_ce_ptr, 
		"type", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpStat_ce_ptr, 
		"device", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpStat_ce_ptr, 
		"mode", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpStat_ce_ptr, 
		"aTime", 5, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpStat_ce_ptr, 
		"mTime", 5, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpStat_ce_ptr, 
		"length", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpStat_ce_ptr, 
		"name", 4, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpStat_ce_ptr, 
		"uid", 3, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpStat_ce_ptr, 
		"gid", 3, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpStat_ce_ptr, 
		"muid", 4, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_class_constant_long(IxpStat_ce_ptr,
			"DMEXEC", 6,
			P9_DMEXEC TSRMLS_CC);
	zend_declare_class_constant_long(IxpStat_ce_ptr,
			"DMWRITE", 7,
			P9_DMWRITE TSRMLS_CC);
	zend_declare_class_constant_long(IxpStat_ce_ptr,
			"DMREAD", 6,
			P9_DMREAD TSRMLS_CC);

	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMDIR", 5,
			P9_DMDIR TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMAPPEND", 8, 
			P9_DMAPPEND TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMEXCL", 6,
			P9_DMEXCL TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMMOUNT", 7,
			P9_DMMOUNT TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMAUTH", 6,
			P9_DMAUTH TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMTMP", 5,
			P9_DMTMP TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMSYMLINK", 9,
			P9_DMSYMLINK TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMDEVICE", 8,
			P9_DMDEVICE TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMNAMEDPIPE", 11,
			P9_DMNAMEDPIPE TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMSOCKET", 8,
			P9_DMSOCKET TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMSETUID", 8,
			P9_DMSETUID TSRMLS_CC);
	zend_declare_class_constant_double(IxpStat_ce_ptr,
			"DMSETGID", 8,
			P9_DMSETGID TSRMLS_CC);
}
/* }}} Class IxpStat */


/* {{{ Class IxpQid */

static zend_object_handlers PHP_IxpQid_handlers;

static void PHP_IxpQid_object_free(void *object TSRMLS_DC)
{
	PHP_IxpQid *intern = (PHP_IxpQid *)object;

	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);
}

static zend_object_value PHP_IxpQid_object_new_ex(zend_class_entry *class_type, PHP_IxpQid **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpQid *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpQid));
	memset(intern, 0, sizeof(PHP_IxpQid));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpQid_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpQid_handlers;
	return retval;
}

static zend_object_value PHP_IxpQid_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpQid *tmp;
	return PHP_IxpQid_object_new_ex(class_type, &tmp TSRMLS_DC);
}

static zend_function_entry IxpQid_methods[] = {
	{ NULL, NULL, NULL }
};

static void class_init_IxpQid(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpQid", IxpQid_methods);
	IxpQid_ce_ptr = zend_register_internal_class(&ce);
	IxpQid_ce_ptr->create_object = PHP_IxpQid_object_new;
	memcpy(&PHP_IxpQid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpQid_handlers.clone_obj = NULL;

	zend_declare_property_long(IxpQid_ce_ptr, 
		"type", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);
	zend_declare_property_long(IxpQid_ce_ptr, 
		"version", 7, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);
	zend_declare_property_long(IxpQid_ce_ptr, 
		"path", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTDIR", 5,
			P9_QTDIR TSRMLS_CC);
	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTAPPEND", 8,
			P9_QTAPPEND TSRMLS_CC);
	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTEXCL", 6,
			P9_QTEXCL TSRMLS_CC);
	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTMOUNT", 7,
			P9_QTMOUNT TSRMLS_CC);
	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTAUTH", 6,
			P9_QTAUTH TSRMLS_CC);
	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTTMP", 5,
			P9_QTTMP TSRMLS_CC);
	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTSYMLINK", 9,
			P9_QTSYMLINK TSRMLS_CC);
	zend_declare_class_constant_long(IxpQid_ce_ptr,
			"QTFILE", 6,
			P9_QTFILE TSRMLS_CC);

}
/* }}} Class IxpQid */


/* {{{ Class IxpConn */

static zend_object_handlers PHP_IxpConn_handlers;

static void PHP_IxpConn_object_free(void *object TSRMLS_DC)
{
	PHP_IxpConn *intern = (PHP_IxpConn *)object;

	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);

	efree(object);
}

static zend_object_value PHP_IxpConn_object_new_ex(zend_class_entry *class_type, PHP_IxpConn **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpConn *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpConn));
	memset(intern, 0, sizeof(PHP_IxpConn));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpConn_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpConn_handlers;
	return retval;
}

static zend_object_value PHP_IxpConn_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpConn *tmp;
	return PHP_IxpConn_object_new_ex(class_type, &tmp TSRMLS_DC);
}

/* {{{ proto void serve() */
PHP_METHOD(IxpConn, serve)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpConn_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpConn *object = FETCH_IxpConn(_this_zval);

	serve_9pcon(object->ptr);
}
/* }}} serve */

/* {{{ proto void hangup() */
PHP_METHOD(IxpConn, hangup)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpConn_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpConn *object = FETCH_IxpConn(_this_zval);

	ixp_hangup(object->ptr);
}
/* }}} serve */

static zend_function_entry IxpConn_methods[] = {
	PHP_ME(IxpConn, serve, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpConn, hangup, NULL, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpConn(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpConn", IxpConn_methods);
	IxpConn_ce_ptr = zend_register_internal_class(&ce);
	IxpConn_ce_ptr->create_object = PHP_IxpConn_object_new;
	memcpy(&PHP_IxpConn_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpConn_handlers.clone_obj = NULL;

	zend_declare_property_long(IxpConn_ce_ptr, 
		"closed", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);
}
/* }}} Class IxpConn */


/* {{{ Class IxpFid */

static zend_object_handlers PHP_IxpFid_handlers;

static void PHP_IxpFid_object_free(void *object TSRMLS_DC)
{
	PHP_IxpFid *intern = (PHP_IxpFid *)object;

	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);

	efree(object);
}

static zend_object_value PHP_IxpFid_object_new_ex(zend_class_entry *class_type, PHP_IxpFid **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpFid *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpFid));
	memset(intern, 0, sizeof(PHP_IxpFid));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpFid_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpFid_handlers;
	return retval;
}

static zend_object_value PHP_IxpFid_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpFid *tmp;
	return PHP_IxpFid_object_new_ex(class_type, &tmp TSRMLS_DC);
}

/* {{{ proto object getQid() */
PHP_METHOD(IxpFid, getQid)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	int ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpFid_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpFid *object = FETCH_IxpFid(_this_zval);

	object_instantiate(IxpQid_ce_ptr, return_value);
	PHP_IxpQid_initialize(return_value, object->ptr->qid);
}
/* }}} getQid */

static zend_function_entry IxpFid_methods[] = {
	PHP_ME(IxpFid, getQid, NULL, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpFid(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpFid", IxpFid_methods);
	IxpFid_ce_ptr = zend_register_internal_class(&ce);
	IxpFid_ce_ptr->create_object = PHP_IxpConn_object_new;
	memcpy(&PHP_IxpFid_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpFid_handlers.clone_obj = NULL;

	zend_declare_property_string(IxpFid_ce_ptr, 
		"uid", 3, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFid_ce_ptr, 
		"fid", 3, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_double(IxpFid_ce_ptr, 
		"omode", 5, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);
}
/* }}} Class IxpFid */


/* {{{ Class IxpRequest */

static zend_object_handlers PHP_IxpRequest_handlers;

static void PHP_IxpRequest_object_free(void *object TSRMLS_DC)
{
	PHP_IxpRequest *intern = (PHP_IxpRequest *)object;

	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);

	efree(object);
}

static zend_object_value PHP_IxpRequest_object_new_ex(zend_class_entry *class_type, PHP_IxpRequest **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpRequest *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpRequest));
	memset(intern, 0, sizeof(PHP_IxpRequest));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpRequest_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpRequest_handlers;
	return retval;
}

static zend_object_value PHP_IxpRequest_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpRequest *tmp;
	return PHP_IxpRequest_object_new_ex(class_type, &tmp TSRMLS_DC);
}

/* {{{ proto void respond(string error) */
PHP_METHOD(IxpRequest, respond)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	const char * error = NULL;
	int error_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, IxpRequest_ce_ptr, &error, &error_len) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	php_error(E_WARNING, "respond: not yet implemented"); RETURN_FALSE;

}
/* }}} respond */

static zend_function_entry IxpRequest_methods[] = {
	PHP_ME(IxpRequest, respond, IxpRequest__respond_args, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpRequest(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpRequest", IxpRequest_methods);
	IxpRequest_ce_ptr = zend_register_internal_class(&ce);
	IxpRequest_ce_ptr->create_object = PHP_IxpConn_object_new;
	memcpy(&PHP_IxpRequest_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpRequest_handlers.clone_obj = NULL;

	zend_declare_property_null(IxpRequest_ce_ptr, 
		"fid", 3, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_null(IxpRequest_ce_ptr, 
		"ifcall", 6, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_null(IxpRequest_ce_ptr, 
		"ofcall", 6, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_null(IxpRequest_ce_ptr, 
		"newfid", 6, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_null(IxpRequest_ce_ptr, 
		"oldreq", 6, 
		ZEND_ACC_PUBLIC TSRMLS_DC);
}
/* }}} Class IxpRequest */


/* {{{ Class IxpFcall */

static zend_object_handlers PHP_IxpFcall_handlers;

static void PHP_IxpFcall_object_free(void *object TSRMLS_DC)
{
	PHP_IxpFcall *intern = (PHP_IxpFcall *)object;

	zend_hash_destroy(intern->obj.properties);
	FREE_HASHTABLE(intern->obj.properties);

	efree(object);
}

static zend_object_value PHP_IxpFcall_object_new_ex(zend_class_entry *class_type, PHP_IxpFcall **object TSRMLS_DC)
{
	zend_object_value retval;
	PHP_IxpFcall *intern;
	zval *tmp;

	intern = malloc(sizeof(PHP_IxpFcall));
	memset(intern, 0, sizeof(PHP_IxpFcall));
	intern->obj.ce = class_type;
	*object = intern;

	ALLOC_HASHTABLE(intern->obj.properties);
	zend_hash_init(intern->obj.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->obj.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) PHP_IxpFcall_object_free, NULL TSRMLS_CC);
	retval.handlers = &PHP_IxpFcall_handlers;
	return retval;
}

static zend_object_value PHP_IxpFcall_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_IxpFcall *tmp;
	return PHP_IxpFcall_object_new_ex(class_type, &tmp TSRMLS_DC);
}

/* {{{ proto void getQid() */
PHP_METHOD(IxpFcall, getQid)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	const char * error = NULL;
	int error_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpFcall_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	php_error(E_WARNING, "getQid: not yet implemented"); RETURN_FALSE;

}
/* }}} getQid */

/* {{{ proto void getAQid() */
PHP_METHOD(IxpFcall, getAQid)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	const char * error = NULL;
	int error_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpFcall_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	php_error(E_WARNING, "getAQid: not yet implemented"); RETURN_FALSE;

}
/* }}} getAQid */

/* {{{ proto void getWQid() */
PHP_METHOD(IxpFcall, getWQid)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;
	const char * error = NULL;
	int error_len = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpFcall_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	php_error(E_WARNING, "getWQid: not yet implemented"); RETURN_FALSE;

}
/* }}} getWQid */

static zend_function_entry IxpFcall_methods[] = {
	PHP_ME(IxpFcall, getQid, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpFcall, getAQid, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpFcall, getWQid, NULL, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpFcall(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpFcall", IxpFcall_methods);
	IxpFcall_ce_ptr = zend_register_internal_class(&ce);
	IxpFcall_ce_ptr->create_object = PHP_IxpConn_object_new;
	memcpy(&PHP_IxpFcall_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	PHP_IxpFcall_handlers.clone_obj = NULL;

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"type", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"tag", 3, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"fid", 3, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"msize", 5, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"version", 7, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"oldtag", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"ename", 5, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"iounit", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"afid", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"uname", 5, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"aname", 5, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"perm", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"name", 4, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"mode", 4, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"newfid", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"nwname", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"wname", 5, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"nwqid", 5, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_double(IxpFcall_ce_ptr, 
		"offset", 6, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"count", 5, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"data", 4, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_long(IxpFcall_ce_ptr, 
		"nstat", 5, 0, 
		ZEND_ACC_PUBLIC TSRMLS_DC);

	zend_declare_property_string(IxpFcall_ce_ptr, 
		"stat", 4, "", 
		ZEND_ACC_PUBLIC TSRMLS_DC);
}

/* }}} Class IxpFcall */


/* {{{ Class IxpServer */

/* {{{ proto void construct(string address, object IxpCallbacks[, object IxpServerCallbacks]) */
PHP_METHOD(IxpServer, __construct)
{
	zend_class_entry * _this_ce;
	zval * _this_zval;
	IxpConn *conn;
	
	char * address = NULL;
	int address_len = 0;
	int sock;

	zval * IxpCallbacks = NULL;
	zval * IxpServerCallbacks = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sO|O", &address, &address_len, &IxpCallbacks, IxpCallbacks_ce_ptr, &IxpServerCallbacks, IxpServerCallbacks_ce_ptr) == FAILURE) { 
		return;
	}

	_this_zval = getThis();
	_this_ce = Z_OBJCE_P(_this_zval);
	PHP_IxpServer *object = FETCH_IxpServer(_this_zval);
	
	IxpServer this_server = {
		.aux		= IxpCallbacks
	};

	Ixp9Srv this_srv = {
		.aux		= IxpCallbacks,
		.open		= PHP_IxpCallbacks_open_marshal,
		.walk		= PHP_IxpCallbacks_walk_marshal,
		.read		= PHP_IxpCallbacks_read_marshal,
		.stat		= PHP_IxpCallbacks_stat_marshal,
		.write		= PHP_IxpCallbacks_write_marshal,
		.clunk		= PHP_IxpCallbacks_clunk_marshal,
		.flush		= PHP_IxpCallbacks_flush_marshal,
		.attach		= PHP_IxpCallbacks_attach_marshal,
		.create		= PHP_IxpCallbacks_create_marshal,
		.remove		= PHP_IxpCallbacks_remove_marshal,
		.freefid	= PHP_IxpCallbacks_freefid_marshal
	};

	sock = ixp_announce(address);
	if (sock < 0) {
		php_error(E_WARNING, ixp_errbuf());
		RETURN_FALSE;
	}

	conn = ixp_listen(&this_server, sock, &this_srv, PHP_IxpServerCallbacks_read_marshal, PHP_IxpServerCallbacks_close_marshal);
	object->ptr = &this_server;
}
/* }}} __construct */

/* {{{ proto void start() */
PHP_METHOD(IxpServer, start)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpServer_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	php_error(E_WARNING, "start: not yet implemented"); RETURN_FALSE;
}
/* }}} start */

/* {{{ proto void stop() */
PHP_METHOD(IxpServer, stop)
{
	zend_class_entry * _this_ce;
	zval * _this_zval = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, IxpServer_ce_ptr) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	php_error(E_WARNING, "stop: not yet implemented"); RETURN_FALSE;

}
/* }}} stop */

static zend_function_entry IxpServer_methods[] = {
	PHP_ME(IxpServer, __construct, IxpServer____construct_args, ZEND_ACC_PUBLIC)
	PHP_ME(IxpServer, start, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(IxpServer, stop, NULL, ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

static void class_init_IxpServer(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "IxpServer", IxpServer_methods);
	IxpServer_ce_ptr = zend_register_internal_class(&ce);
}
/* }}} Class IxpServer */

/* {{{ ixp_functions[] */
function_entry ixp_functions[] = {
	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ ixp_module_entry */
zend_module_entry ixp_module_entry = {
	STANDARD_MODULE_HEADER,
	"ixp",
	ixp_functions,
	PHP_MINIT(ixp),     /* Replace with NULL if there is nothing to do at php startup   */ 
	PHP_MSHUTDOWN(ixp), /* Replace with NULL if there is nothing to do at php shutdown  */
	PHP_RINIT(ixp),     /* Replace with NULL if there is nothing to do at request start */
	PHP_RSHUTDOWN(ixp), /* Replace with NULL if there is nothing to do at request end   */
	PHP_MINFO(ixp),
	"0.0.1", 
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_IXP
ZEND_GET_MODULE(ixp)
#endif

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(ixp)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(ixp)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(ixp)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(ixp)
{
	php_info_print_box_start(0);
	php_printf("<p>PHP bindings to the 9P protocol</p>\n");
	php_printf("<p>Version 0.0.1devel (2007-06-11)</p>\n");
	php_printf("<p><b>Authors:</b></p>\n");
	php_printf("<p>Anant Narayanan &lt;anant@php.net&gt; (lead)</p>\n");
	php_info_print_box_end();
}
/* }}} */

#endif /* HAVE_IXP */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 
 * vim<600: noet sw=4 ts=4
 */
