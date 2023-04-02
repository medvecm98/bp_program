// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ip_map.proto

#include "ip_map.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace np2ps {
PROTOBUF_CONSTEXPR IpWrapper::IpWrapper(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_._has_bits_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}
  , /*decltype(_impl_.eax_key_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.rsa_public_key_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.publicid_)*/uint64_t{0u}
  , /*decltype(_impl_.ipv4_)*/0u
  , /*decltype(_impl_.port_)*/0u
  , /*decltype(_impl_.relay_flag_)*/false} {}
struct IpWrapperDefaultTypeInternal {
  PROTOBUF_CONSTEXPR IpWrapperDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~IpWrapperDefaultTypeInternal() {}
  union {
    IpWrapper _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 IpWrapperDefaultTypeInternal _IpWrapper_default_instance_;
PROTOBUF_CONSTEXPR IpMap::IpMap(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_._has_bits_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}
  , /*decltype(_impl_.wrapper_map_)*/{}
  , /*decltype(_impl_.rsa_private_key_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.my_ip()_)*/nullptr
  , /*decltype(_impl_.my_public_id_)*/uint64_t{0u}} {}
struct IpMapDefaultTypeInternal {
  PROTOBUF_CONSTEXPR IpMapDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~IpMapDefaultTypeInternal() {}
  union {
    IpMap _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 IpMapDefaultTypeInternal _IpMap_default_instance_;
}  // namespace np2ps
static ::_pb::Metadata file_level_metadata_ip_5fmap_2eproto[2];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_ip_5fmap_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_ip_5fmap_2eproto = nullptr;

const uint32_t TableStruct_ip_5fmap_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _impl_._has_bits_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _impl_.publicid_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _impl_.ipv4_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _impl_.port_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _impl_.relay_flag_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _impl_.eax_key_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpWrapper, _impl_.rsa_public_key_),
  ~0u,
  ~0u,
  ~0u,
  ~0u,
  0,
  1,
  PROTOBUF_FIELD_OFFSET(::np2ps::IpMap, _impl_._has_bits_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpMap, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::np2ps::IpMap, _impl_.my_ip()_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpMap, _impl_.my_public_id_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpMap, _impl_.rsa_private_key_),
  PROTOBUF_FIELD_OFFSET(::np2ps::IpMap, _impl_.wrapper_map_),
  ~0u,
  ~0u,
  0,
  ~0u,
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 12, -1, sizeof(::np2ps::IpWrapper)},
  { 18, 28, -1, sizeof(::np2ps::IpMap)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::np2ps::_IpWrapper_default_instance_._instance,
  &::np2ps::_IpMap_default_instance_._instance,
};

const char descriptor_table_protodef_ip_5fmap_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\014ip_map.proto\022\005np2ps\"\237\001\n\tIpWrapper\022\020\n\010p"
  "ublicID\030\005 \001(\004\022\014\n\004ipv4\030\n \001(\r\022\014\n\004port\030\024 \001("
  "\r\022\022\n\nrelay_flag\030\031 \001(\010\022\024\n\007eax_key\030\036 \001(\tH\000"
  "\210\001\001\022\033\n\016rsa_public_key\030( \001(\tH\001\210\001\001B\n\n\010_eax"
  "_keyB\021\n\017_rsa_public_key\"\227\001\n\005IpMap\022\037\n\005my_"
  "IP\030\n \001(\0132\020.np2ps.IpWrapper\022\024\n\014my_public_"
  "ID\030\024 \001(\004\022\034\n\017rsa_private_key\030\036 \001(\tH\000\210\001\001\022%"
  "\n\013wrapper_map\030( \003(\0132\020.np2ps.IpWrapperB\022\n"
  "\020_rsa_private_keyb\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_ip_5fmap_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_ip_5fmap_2eproto = {
    false, false, 345, descriptor_table_protodef_ip_5fmap_2eproto,
    "ip_map.proto",
    &descriptor_table_ip_5fmap_2eproto_once, nullptr, 0, 2,
    schemas, file_default_instances, TableStruct_ip_5fmap_2eproto::offsets,
    file_level_metadata_ip_5fmap_2eproto, file_level_enum_descriptors_ip_5fmap_2eproto,
    file_level_service_descriptors_ip_5fmap_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_ip_5fmap_2eproto_getter() {
  return &descriptor_table_ip_5fmap_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_ip()_5fmap_2eproto(&descriptor_table_ip_5fmap_2eproto);
namespace np2ps {

// ===================================================================

class IpWrapper::_Internal {
 public:
  using HasBits = decltype(std::declval<IpWrapper>()._impl_._has_bits_);
  static void set_has_eax_key(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_rsa_public_key(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

IpWrapper::IpWrapper(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:np2ps.IpWrapper)
}
IpWrapper::IpWrapper(const IpWrapper& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  IpWrapper* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.eax_key_){}
    , decltype(_impl_.rsa_public_key_){}
    , decltype(_impl_.publicid_){}
    , decltype(_impl_.ipv4_){}
    , decltype(_impl_.port_){}
    , decltype(_impl_.relay_flag_){}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.eax_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.eax_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (from._internal_has_eax_key()) {
    _this->_impl_.eax_key_.Set(from._internal_eax_key(), 
      _this->GetArenaForAllocation());
  }
  _impl_.rsa_public_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.rsa_public_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (from._internal_has_rsa_public_key()) {
    _this->_impl_.rsa_public_key_.Set(from._internal_rsa_public_key(), 
      _this->GetArenaForAllocation());
  }
  ::memcpy(&_impl_.publicid_, &from._impl_.publicid_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.relay_flag_) -
    reinterpret_cast<char*>(&_impl_.publicid_)) + sizeof(_impl_.relay_flag_));
  // @@protoc_insertion_point(copy_constructor:np2ps.IpWrapper)
}

inline void IpWrapper::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.eax_key_){}
    , decltype(_impl_.rsa_public_key_){}
    , decltype(_impl_.publicid_){uint64_t{0u}}
    , decltype(_impl_.ipv4_){0u}
    , decltype(_impl_.port_){0u}
    , decltype(_impl_.relay_flag_){false}
  };
  _impl_.eax_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.eax_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  _impl_.rsa_public_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.rsa_public_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

IpWrapper::~IpWrapper() {
  // @@protoc_insertion_point(destructor:np2ps.IpWrapper)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void IpWrapper::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.eax_key_.Destroy();
  _impl_.rsa_public_key_.Destroy();
}

void IpWrapper::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void IpWrapper::Clear() {
// @@protoc_insertion_point(message_clear_start:np2ps.IpWrapper)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _impl_.eax_key_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      _impl_.rsa_public_key_.ClearNonDefaultToEmpty();
    }
  }
  ::memset(&_impl_.publicid_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.relay_flag_) -
      reinterpret_cast<char*>(&_impl_.publicid_)) + sizeof(_impl_.relay_flag_));
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* IpWrapper::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // uint64 publicID = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 40)) {
          _impl_.publicid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint32 ipv4 = 10;
      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 80)) {
          _impl_.ipv4_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint32 port = 20;
      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 160)) {
          _impl_.port_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // bool relay_flag = 25;
      case 25:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 200)) {
          _impl_.relay_flag_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional string eax_key = 30;
      case 30:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 242)) {
          auto str = _internal_mutable_eax_key();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "np2ps.IpWrapper.eax_key"));
        } else
          goto handle_unusual;
        continue;
      // optional string rsa_public_key = 40;
      case 40:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 66)) {
          auto str = _internal_mutable_rsa_public_key();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "np2ps.IpWrapper.rsa_public_key"));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _impl_._has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* IpWrapper::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:np2ps.IpWrapper)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // uint64 publicID = 5;
  if (this->_internal_publicid() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt64ToArray(5, this->_internal_publicid(), target);
  }

  // uint32 ipv4 = 10;
  if (this->_internal_ipv4() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(10, this->_internal_ipv4(), target);
  }

  // uint32 port = 20;
  if (this->_internal_port() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(20, this->_internal_port(), target);
  }

  // bool relay_flag = 25;
  if (this->_internal_relay_flag() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteBoolToArray(25, this->_internal_relay_flag(), target);
  }

  // optional string eax_key = 30;
  if (_internal_has_eax_key()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_eax_key().data(), static_cast<int>(this->_internal_eax_key().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "np2ps.IpWrapper.eax_key");
    target = stream->WriteStringMaybeAliased(
        30, this->_internal_eax_key(), target);
  }

  // optional string rsa_public_key = 40;
  if (_internal_has_rsa_public_key()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_rsa_public_key().data(), static_cast<int>(this->_internal_rsa_public_key().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "np2ps.IpWrapper.rsa_public_key");
    target = stream->WriteStringMaybeAliased(
        40, this->_internal_rsa_public_key(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:np2ps.IpWrapper)
  return target;
}

size_t IpWrapper::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:np2ps.IpWrapper)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    // optional string eax_key = 30;
    if (cached_has_bits & 0x00000001u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_eax_key());
    }

    // optional string rsa_public_key = 40;
    if (cached_has_bits & 0x00000002u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_rsa_public_key());
    }

  }
  // uint64 publicID = 5;
  if (this->_internal_publicid() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt64SizePlusOne(this->_internal_publicid());
  }

  // uint32 ipv4 = 10;
  if (this->_internal_ipv4() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(this->_internal_ipv4());
  }

  // uint32 port = 20;
  if (this->_internal_port() != 0) {
    total_size += 2 +
      ::_pbi::WireFormatLite::UInt32Size(
        this->_internal_port());
  }

  // bool relay_flag = 25;
  if (this->_internal_relay_flag() != 0) {
    total_size += 2 + 1;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData IpWrapper::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    IpWrapper::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*IpWrapper::GetClassData() const { return &_class_data_; }


void IpWrapper::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<IpWrapper*>(&to_msg);
  auto& from = static_cast<const IpWrapper&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:np2ps.IpWrapper)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._impl_._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _this->_internal_set_eax_key(from._internal_eax_key());
    }
    if (cached_has_bits & 0x00000002u) {
      _this->_internal_set_rsa_public_key(from._internal_rsa_public_key());
    }
  }
  if (from._internal_publicid() != 0) {
    _this->_internal_set_publicid(from._internal_publicid());
  }
  if (from._internal_ipv4() != 0) {
    _this->_internal_set_ipv4(from._internal_ipv4());
  }
  if (from._internal_port() != 0) {
    _this->_internal_set_port(from._internal_port());
  }
  if (from._internal_relay_flag() != 0) {
    _this->_internal_set_relay_flag(from._internal_relay_flag());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void IpWrapper::CopyFrom(const IpWrapper& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:np2ps.IpWrapper)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool IpWrapper::IsInitialized() const {
  return true;
}

void IpWrapper::InternalSwap(IpWrapper* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.eax_key_, lhs_arena,
      &other->_impl_.eax_key_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.rsa_public_key_, lhs_arena,
      &other->_impl_.rsa_public_key_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(IpWrapper, _impl_.relay_flag_)
      + sizeof(IpWrapper::_impl_.relay_flag_)
      - PROTOBUF_FIELD_OFFSET(IpWrapper, _impl_.publicid_)>(
          reinterpret_cast<char*>(&_impl_.publicid_),
          reinterpret_cast<char*>(&other->_impl_.publicid_));
}

::PROTOBUF_NAMESPACE_ID::Metadata IpWrapper::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_ip_5fmap_2eproto_getter, &descriptor_table_ip_5fmap_2eproto_once,
      file_level_metadata_ip_5fmap_2eproto[0]);
}

// ===================================================================

class IpMap::_Internal {
 public:
  using HasBits = decltype(std::declval<IpMap>()._impl_._has_bits_);
  static const ::np2ps::IpWrapper& my_ip()(const IpMap* msg);
  static void set_has_rsa_private_key(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

const ::np2ps::IpWrapper&
IpMap::_Internal::my_ip()(const IpMap* msg) {
  return *msg->_impl_.my_ip()_;
}
IpMap::IpMap(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:np2ps.IpMap)
}
IpMap::IpMap(const IpMap& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  IpMap* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.wrapper_map_){from._impl_.wrapper_map_}
    , decltype(_impl_.rsa_private_key_){}
    , decltype(_impl_.my_ip()_){nullptr}
    , decltype(_impl_.my_public_id_){}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.rsa_private_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.rsa_private_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (from._internal_has_rsa_private_key()) {
    _this->_impl_.rsa_private_key_.Set(from._internal_rsa_private_key(), 
      _this->GetArenaForAllocation());
  }
  if (from._internal_has_my_ip()()) {
    _this->_impl_.my_ip()_ = new ::np2ps::IpWrapper(*from._impl_.my_ip()_);
  }
  _this->_impl_.my_public_id_ = from._impl_.my_public_id_;
  // @@protoc_insertion_point(copy_constructor:np2ps.IpMap)
}

inline void IpMap::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.wrapper_map_){arena}
    , decltype(_impl_.rsa_private_key_){}
    , decltype(_impl_.my_ip()_){nullptr}
    , decltype(_impl_.my_public_id_){uint64_t{0u}}
  };
  _impl_.rsa_private_key_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.rsa_private_key_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

IpMap::~IpMap() {
  // @@protoc_insertion_point(destructor:np2ps.IpMap)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void IpMap::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.wrapper_map_.~RepeatedPtrField();
  _impl_.rsa_private_key_.Destroy();
  if (this != internal_default_instance()) delete _impl_.my_ip()_;
}

void IpMap::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void IpMap::Clear() {
// @@protoc_insertion_point(message_clear_start:np2ps.IpMap)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.wrapper_map_.Clear();
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    _impl_.rsa_private_key_.ClearNonDefaultToEmpty();
  }
  if (GetArenaForAllocation() == nullptr && _impl_.my_ip()_ != nullptr) {
    delete _impl_.my_ip()_;
  }
  _impl_.my_ip()_ = nullptr;
  _impl_.my_public_id_ = uint64_t{0u};
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* IpMap::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // .np2ps.IpWrapper my_ip() = 10;
      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 82)) {
          ptr = ctx->ParseMessage(_internal_mutable_my_ip()(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint64 my_public_ID = 20;
      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 160)) {
          _impl_.my_public_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // optional string rsa_private_key = 30;
      case 30:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 242)) {
          auto str = _internal_mutable_rsa_private_key();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "np2ps.IpMap.rsa_private_key"));
        } else
          goto handle_unusual;
        continue;
      // repeated .np2ps.IpWrapper wrapper_map = 40;
      case 40:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 66)) {
          ptr -= 2;
          do {
            ptr += 2;
            ptr = ctx->ParseMessage(_internal_add_wrapper_map(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<322>(ptr));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _impl_._has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* IpMap::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:np2ps.IpMap)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // .np2ps.IpWrapper my_ip() = 10;
  if (this->_internal_has_my_ip()()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(10, _Internal::my_ip()(this),
        _Internal::my_ip()(this).GetCachedSize(), target, stream);
  }

  // uint64 my_public_ID = 20;
  if (this->_internal_my_public_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt64ToArray(20, this->_internal_my_public_id(), target);
  }

  // optional string rsa_private_key = 30;
  if (_internal_has_rsa_private_key()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_rsa_private_key().data(), static_cast<int>(this->_internal_rsa_private_key().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "np2ps.IpMap.rsa_private_key");
    target = stream->WriteStringMaybeAliased(
        30, this->_internal_rsa_private_key(), target);
  }

  // repeated .np2ps.IpWrapper wrapper_map = 40;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_wrapper_map_size()); i < n; i++) {
    const auto& repfield = this->_internal_wrapper_map(i);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
        InternalWriteMessage(40, repfield, repfield.GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:np2ps.IpMap)
  return target;
}

size_t IpMap::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:np2ps.IpMap)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .np2ps.IpWrapper wrapper_map = 40;
  total_size += 2UL * this->_internal_wrapper_map_size();
  for (const auto& msg : this->_impl_.wrapper_map_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  // optional string rsa_private_key = 30;
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 2 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_rsa_private_key());
  }

  // .np2ps.IpWrapper my_ip() = 10;
  if (this->_internal_has_my_ip()()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *_impl_.my_ip()_);
  }

  // uint64 my_public_ID = 20;
  if (this->_internal_my_public_id() != 0) {
    total_size += 2 +
      ::_pbi::WireFormatLite::UInt64Size(
        this->_internal_my_public_id());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData IpMap::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    IpMap::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*IpMap::GetClassData() const { return &_class_data_; }


void IpMap::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<IpMap*>(&to_msg);
  auto& from = static_cast<const IpMap&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:np2ps.IpMap)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_impl_.wrapper_map_.MergeFrom(from._impl_.wrapper_map_);
  if (from._internal_has_rsa_private_key()) {
    _this->_internal_set_rsa_private_key(from._internal_rsa_private_key());
  }
  if (from._internal_has_my_ip()()) {
    _this->_internal_mutable_my_ip()()->::np2ps::IpWrapper::MergeFrom(
        from._internal_my_ip()());
  }
  if (from._internal_my_public_id() != 0) {
    _this->_internal_set_my_public_id(from._internal_my_public_id());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void IpMap::CopyFrom(const IpMap& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:np2ps.IpMap)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool IpMap::IsInitialized() const {
  return true;
}

void IpMap::InternalSwap(IpMap* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  _impl_.wrapper_map_.InternalSwap(&other->_impl_.wrapper_map_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.rsa_private_key_, lhs_arena,
      &other->_impl_.rsa_private_key_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(IpMap, _impl_.my_public_id_)
      + sizeof(IpMap::_impl_.my_public_id_)
      - PROTOBUF_FIELD_OFFSET(IpMap, _impl_.my_ip()_)>(
          reinterpret_cast<char*>(&_impl_.my_ip()_),
          reinterpret_cast<char*>(&other->_impl_.my_ip()_));
}

::PROTOBUF_NAMESPACE_ID::Metadata IpMap::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_ip_5fmap_2eproto_getter, &descriptor_table_ip_5fmap_2eproto_once,
      file_level_metadata_ip_5fmap_2eproto[1]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace np2ps
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::np2ps::IpWrapper*
Arena::CreateMaybeMessage< ::np2ps::IpWrapper >(Arena* arena) {
  return Arena::CreateMessageInternal< ::np2ps::IpWrapper >(arena);
}
template<> PROTOBUF_NOINLINE ::np2ps::IpMap*
Arena::CreateMaybeMessage< ::np2ps::IpMap >(Arena* arena) {
  return Arena::CreateMessageInternal< ::np2ps::IpMap >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
