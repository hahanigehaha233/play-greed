// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: info.proto

#ifndef PROTOBUF_info_2eproto__INCLUDED
#define PROTOBUF_info_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace pubsub {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_info_2eproto();
void protobuf_AssignDesc_info_2eproto();
void protobuf_ShutdownFile_info_2eproto();

class SystemQuery;
class SystemAns;

// ===================================================================

class SystemQuery : public ::google::protobuf::Message {
 public:
  SystemQuery();
  virtual ~SystemQuery();

  SystemQuery(const SystemQuery& from);

  inline SystemQuery& operator=(const SystemQuery& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const SystemQuery& default_instance();

  void Swap(SystemQuery* other);

  // implements Message ----------------------------------------------

  SystemQuery* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SystemQuery& from);
  void MergeFrom(const SystemQuery& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string cmd = 2;
  inline bool has_cmd() const;
  inline void clear_cmd();
  static const int kCmdFieldNumber = 2;
  inline const ::std::string& cmd() const;
  inline void set_cmd(const ::std::string& value);
  inline void set_cmd(const char* value);
  inline void set_cmd(const char* value, size_t size);
  inline ::std::string* mutable_cmd();
  inline ::std::string* release_cmd();
  inline void set_allocated_cmd(::std::string* cmd);

  // required string topic = 3;
  inline bool has_topic() const;
  inline void clear_topic();
  static const int kTopicFieldNumber = 3;
  inline const ::std::string& topic() const;
  inline void set_topic(const ::std::string& value);
  inline void set_topic(const char* value);
  inline void set_topic(const char* value, size_t size);
  inline ::std::string* mutable_topic();
  inline ::std::string* release_topic();
  inline void set_allocated_topic(::std::string* topic);

  // @@protoc_insertion_point(class_scope:pubsub.SystemQuery)
 private:
  inline void set_has_cmd();
  inline void clear_has_cmd();
  inline void set_has_topic();
  inline void clear_has_topic();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* cmd_;
  ::std::string* topic_;
  friend void  protobuf_AddDesc_info_2eproto();
  friend void protobuf_AssignDesc_info_2eproto();
  friend void protobuf_ShutdownFile_info_2eproto();

  void InitAsDefaultInstance();
  static SystemQuery* default_instance_;
};
// -------------------------------------------------------------------

class SystemAns : public ::google::protobuf::Message {
 public:
  SystemAns();
  virtual ~SystemAns();

  SystemAns(const SystemAns& from);

  inline SystemAns& operator=(const SystemAns& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const SystemAns& default_instance();

  void Swap(SystemAns* other);

  // implements Message ----------------------------------------------

  SystemAns* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SystemAns& from);
  void MergeFrom(const SystemAns& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string content = 2;
  inline bool has_content() const;
  inline void clear_content();
  static const int kContentFieldNumber = 2;
  inline const ::std::string& content() const;
  inline void set_content(const ::std::string& value);
  inline void set_content(const char* value);
  inline void set_content(const char* value, size_t size);
  inline ::std::string* mutable_content();
  inline ::std::string* release_content();
  inline void set_allocated_content(::std::string* content);

  // @@protoc_insertion_point(class_scope:pubsub.SystemAns)
 private:
  inline void set_has_content();
  inline void clear_has_content();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* content_;
  friend void  protobuf_AddDesc_info_2eproto();
  friend void protobuf_AssignDesc_info_2eproto();
  friend void protobuf_ShutdownFile_info_2eproto();

  void InitAsDefaultInstance();
  static SystemAns* default_instance_;
};
// ===================================================================


// ===================================================================

// SystemQuery

// required string cmd = 2;
inline bool SystemQuery::has_cmd() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SystemQuery::set_has_cmd() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SystemQuery::clear_has_cmd() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SystemQuery::clear_cmd() {
  if (cmd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_->clear();
  }
  clear_has_cmd();
}
inline const ::std::string& SystemQuery::cmd() const {
  // @@protoc_insertion_point(field_get:pubsub.SystemQuery.cmd)
  return *cmd_;
}
inline void SystemQuery::set_cmd(const ::std::string& value) {
  set_has_cmd();
  if (cmd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_ = new ::std::string;
  }
  cmd_->assign(value);
  // @@protoc_insertion_point(field_set:pubsub.SystemQuery.cmd)
}
inline void SystemQuery::set_cmd(const char* value) {
  set_has_cmd();
  if (cmd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_ = new ::std::string;
  }
  cmd_->assign(value);
  // @@protoc_insertion_point(field_set_char:pubsub.SystemQuery.cmd)
}
inline void SystemQuery::set_cmd(const char* value, size_t size) {
  set_has_cmd();
  if (cmd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_ = new ::std::string;
  }
  cmd_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:pubsub.SystemQuery.cmd)
}
inline ::std::string* SystemQuery::mutable_cmd() {
  set_has_cmd();
  if (cmd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    cmd_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:pubsub.SystemQuery.cmd)
  return cmd_;
}
inline ::std::string* SystemQuery::release_cmd() {
  clear_has_cmd();
  if (cmd_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = cmd_;
    cmd_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void SystemQuery::set_allocated_cmd(::std::string* cmd) {
  if (cmd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete cmd_;
  }
  if (cmd) {
    set_has_cmd();
    cmd_ = cmd;
  } else {
    clear_has_cmd();
    cmd_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:pubsub.SystemQuery.cmd)
}

// required string topic = 3;
inline bool SystemQuery::has_topic() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SystemQuery::set_has_topic() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SystemQuery::clear_has_topic() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SystemQuery::clear_topic() {
  if (topic_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    topic_->clear();
  }
  clear_has_topic();
}
inline const ::std::string& SystemQuery::topic() const {
  // @@protoc_insertion_point(field_get:pubsub.SystemQuery.topic)
  return *topic_;
}
inline void SystemQuery::set_topic(const ::std::string& value) {
  set_has_topic();
  if (topic_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    topic_ = new ::std::string;
  }
  topic_->assign(value);
  // @@protoc_insertion_point(field_set:pubsub.SystemQuery.topic)
}
inline void SystemQuery::set_topic(const char* value) {
  set_has_topic();
  if (topic_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    topic_ = new ::std::string;
  }
  topic_->assign(value);
  // @@protoc_insertion_point(field_set_char:pubsub.SystemQuery.topic)
}
inline void SystemQuery::set_topic(const char* value, size_t size) {
  set_has_topic();
  if (topic_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    topic_ = new ::std::string;
  }
  topic_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:pubsub.SystemQuery.topic)
}
inline ::std::string* SystemQuery::mutable_topic() {
  set_has_topic();
  if (topic_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    topic_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:pubsub.SystemQuery.topic)
  return topic_;
}
inline ::std::string* SystemQuery::release_topic() {
  clear_has_topic();
  if (topic_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = topic_;
    topic_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void SystemQuery::set_allocated_topic(::std::string* topic) {
  if (topic_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete topic_;
  }
  if (topic) {
    set_has_topic();
    topic_ = topic;
  } else {
    clear_has_topic();
    topic_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:pubsub.SystemQuery.topic)
}

// -------------------------------------------------------------------

// SystemAns

// required string content = 2;
inline bool SystemAns::has_content() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SystemAns::set_has_content() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SystemAns::clear_has_content() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SystemAns::clear_content() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_->clear();
  }
  clear_has_content();
}
inline const ::std::string& SystemAns::content() const {
  // @@protoc_insertion_point(field_get:pubsub.SystemAns.content)
  return *content_;
}
inline void SystemAns::set_content(const ::std::string& value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set:pubsub.SystemAns.content)
}
inline void SystemAns::set_content(const char* value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set_char:pubsub.SystemAns.content)
}
inline void SystemAns::set_content(const char* value, size_t size) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:pubsub.SystemAns.content)
}
inline ::std::string* SystemAns::mutable_content() {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:pubsub.SystemAns.content)
  return content_;
}
inline ::std::string* SystemAns::release_content() {
  clear_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = content_;
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void SystemAns::set_allocated_content(::std::string* content) {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete content_;
  }
  if (content) {
    set_has_content();
    content_ = content;
  } else {
    clear_has_content();
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:pubsub.SystemAns.content)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace pubsub

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_info_2eproto__INCLUDED