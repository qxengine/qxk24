Pod::Spec.new do |s|
  s.name = "QXK24"
  s.version = "1.0.0-alpha.4"
  s.summary = "QXK24 Universal Kernel"
  s.description = "ALAMTOLOGI - Quranic Science. Founder: Masa Bayu."
  s.homepage = "https://github.com/qxengine/qxk24"
  s.license = { :type => "Apache-2.0" }
  s.author = { "Masa Bayu" => "masaqiubbx@gmail.com" }
  s.source = {
    :git => "https://github.com/qxengine/qxk24.git",
    :tag => s.version.to_s
  }

  s.ios.deployment_target = "15.0"
  s.swift_version = "5.9"
  s.source_files = [
    "include/qxk24/**/*.h",
    "src/**/*.c",
    "platform/ios/*.{h,m,swift}"
  ]
  s.public_header_files = "platform/ios/QXK24Bridge.h"
  s.pod_target_xcconfig = {
    "SWIFT_INCLUDE_PATHS" => "$(PODS_TARGET_SRCROOT)/platform/ios",
    "HEADER_SEARCH_PATHS" => "$(PODS_TARGET_SRCROOT)/include",
    "OTHER_CFLAGS" => "-std=c11 -Wall -Wextra"
  }
end
