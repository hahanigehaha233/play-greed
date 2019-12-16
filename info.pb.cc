// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: info.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "info.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace pubsub {

namespace {

const ::google::protobuf::Descriptor* SystemQuery_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  SystemQuery_reflection_ = NULL;
const ::google::protobuf::Descriptor* SystemAns_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  SystemAns_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_info_2eproto() {
  protobuf_AddDesc_info_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "info.proto");
  GOOGLE_CHECK(file != NULL);
  SystemQuery_descriptor_ = file->message_type(0);
  static const int SystemQuery_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SystemQuery, cmd_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SystemQuery, topic_),
  };
  SystemQuery_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      SystemQuery_descriptor_,
      SystemQuery::default_instance_,
      SystemQuery_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SystemQuery, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SystemQuery, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(SystemQuery));
  SystemAns_descriptor_ = file->message_type(1);
  static const int SystemAns_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SystemAns, content_),
  };
  SystemAns_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      SystemAns_descriptor_,
      SystemAns::default_instance_,
      SystemAns_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SystemAns, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SystemAns, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(SystemAns));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_info_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    SystemQuery_descriptor_, &SystemQuery::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    SystemAns_descriptor_, &SystemAns::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_info_2eproto() {
  delete SystemQuery::default_instance_;
  delete SystemQuery_reflection_;
  delete SystemAns::default_instance_;
  delete SystemAns_reflection_;
}

void protobuf_AddDesc_info_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\ninfo.proto\022\006pubsub\")\n\013SystemQuery\022\013\n\003c"
    "md\030\002 \002(\t\022\r\n\005topic\030\003 \002(\t\"\034\n\tSystemAns\022\017\n\007"
    "content\030\002 \002(\t", 93);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "info.proto", &protobuf_RegisterTypes);
  SystemQuery::default_instance_ = new SystemQuery();
  SystemAns::default_instance_ = new SystemAns();
  SystemQuery::default_instance_->InitAsDefaultInstance();
  SystemAns::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_info_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_info_2eproto {
  StaticDescriptorInitializer_info_2eproto() {
    protobuf_AddDesc_info_2eproto();
  }
} static_descriptor_initializer_info_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int SystemQuery::kCmdFieldNumber;
const int SystemQuery::kTopicFieldNumber;
#endif  // !_MSC_VER

SystemQuery::SystemQuery()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:pubsub.SystemQuery)
}

void SystemQuery::InitAsDefaultInstance() {
}

SystemQuery::SystemQuery(const SystemQuery& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:pubsub.SystemQuery)
}

void SystemQuery::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  cmd_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  topic_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

SystemQuery::~SystemQuery() {
  // @@protoc_insertion_point(destructor:pubsub.SystemQuery)
  SharedDtor();
}

void SystemQuery::SharedDtor() {
  if (cmd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete cmd_;
  }
  if (topic_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete topic_;
  }
  if (this != default_instance_) {
  }
}

void SystemQuery::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SystemQuery::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SystemQuery_descriptor_;
}

const SystemQuery& SystemQuery::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_info_2eproto();
  return *default_instance_;
}

SystemQuery* SystemQuery::default_instance_ = NULL;

SystemQuery* SystemQuery::New() const {
  return new SystemQuery;
}

void SystemQuery::Clear() {
  if (_has_bits_[0 / 32] & 3) {
    if (has_cmd()) {
      if (cmd_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        cmd_->clear();
      }
    }
    if (has_topic()) {
      if (topic_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
        topic_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool SystemQuery::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:pubsub.SystemQuery)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string cmd = 2;
      case 2: {
        if (tag == 18) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_cmd()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->cmd().data(), this->cmd().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "cmd");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_topic;
        break;
      }

      // required string topic = 3;
      case 3: {
        if (tag == 26) {
         parse_topic:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_topic()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->topic().data(), this->topic().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "topic");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:pubsub.SystemQuery)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:pubsub.SystemQuery)
  return false;
#undef DO_
}

void SystemQuery::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:pubsub.SystemQuery)
  // required string cmd = 2;
  if (has_cmd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->cmd().data(), this->cmd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "cmd");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->cmd(), output);
  }

  // required string topic = 3;
  if (has_topic()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->topic().data(), this->topic().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "topic");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->topic(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:pubsub.SystemQuery)
}

::google::protobuf::uint8* SystemQuery::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:pubsub.SystemQuery)
  // required string cmd = 2;
  if (has_cmd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->cmd().data(), this->cmd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "cmd");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->cmd(), target);
  }

  // required string topic = 3;
  if (has_topic()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->topic().data(), this->topic().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "topic");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->topic(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:pubsub.SystemQuery)
  return target;
}

int SystemQuery::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string cmd = 2;
    if (has_cmd()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->cmd());
    }

    // required string topic = 3;
    if (has_topic()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->topic());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SystemQuery::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const SystemQuery* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const SystemQuery*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void SystemQuery::MergeFrom(const SystemQuery& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_cmd()) {
      set_cmd(from.cmd());
    }
    if (from.has_topic()) {
      set_topic(from.topic());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void SystemQuery::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SystemQuery::CopyFrom(const SystemQuery& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SystemQuery::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void SystemQuery::Swap(SystemQuery* other) {
  if (other != this) {
    std::swap(cmd_, other->cmd_);
    std::swap(topic_, other->topic_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata SystemQuery::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = SystemQuery_descriptor_;
  metadata.reflection = SystemQuery_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int SystemAns::kContentFieldNumber;
#endif  // !_MSC_VER

SystemAns::SystemAns()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:pubsub.SystemAns)
}

void SystemAns::InitAsDefaultInstance() {
}

SystemAns::SystemAns(const SystemAns& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:pubsub.SystemAns)
}

void SystemAns::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

SystemAns::~SystemAns() {
  // @@protoc_insertion_point(destructor:pubsub.SystemAns)
  SharedDtor();
}

void SystemAns::SharedDtor() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete content_;
  }
  if (this != default_instance_) {
  }
}

void SystemAns::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SystemAns::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SystemAns_descriptor_;
}

const SystemAns& SystemAns::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_info_2eproto();
  return *default_instance_;
}

SystemAns* SystemAns::default_instance_ = NULL;

SystemAns* SystemAns::New() const {
  return new SystemAns;
}

void SystemAns::Clear() {
  if (has_content()) {
    if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
      content_->clear();
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool SystemAns::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:pubsub.SystemAns)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string content = 2;
      case 2: {
        if (tag == 18) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_content()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->content().data(), this->content().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "content");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:pubsub.SystemAns)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:pubsub.SystemAns)
  return false;
#undef DO_
}

void SystemAns::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:pubsub.SystemAns)
  // required string content = 2;
  if (has_content()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->content().data(), this->content().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "content");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->content(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:pubsub.SystemAns)
}

::google::protobuf::uint8* SystemAns::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:pubsub.SystemAns)
  // required string content = 2;
  if (has_content()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->content().data(), this->content().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "content");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->content(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:pubsub.SystemAns)
  return target;
}

int SystemAns::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string content = 2;
    if (has_content()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->content());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void SystemAns::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const SystemAns* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const SystemAns*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void SystemAns::MergeFrom(const SystemAns& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_content()) {
      set_content(from.content());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void SystemAns::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SystemAns::CopyFrom(const SystemAns& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SystemAns::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void SystemAns::Swap(SystemAns* other) {
  if (other != this) {
    std::swap(content_, other->content_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata SystemAns::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = SystemAns_descriptor_;
  metadata.reflection = SystemAns_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace pubsub

// @@protoc_insertion_point(global_scope)
