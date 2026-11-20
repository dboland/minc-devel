/*
 * Copyright (c) 2016 Daniel Boland <dboland@xs4all.nl>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

typedef enum _WELL_KNOWN_SID_TYPE {
  WinNullSid,
  WinWorldSid,
  WinLocalSid,
  WinCreatorOwnerSid,
  WinCreatorGroupSid,
  WinCreatorOwnerServerSid,
  WinCreatorGroupServerSid,
  WinNtAuthoritySid,
  WinDialupSid,
  WinNetworkSid,
  WinBatchSid,
  WinInteractiveSid,
  WinServiceSid,
  WinAnonymousSid,
  WinProxySid,
  WinEnterpriseControllersSid,
  WinSelfSid,
  WinAuthenticatedUserSid,
  WinRestrictedCodeSid,
  WinTerminalServerSid,
  WinRemoteLogonIdSid,
  WinLogonIdsSid,
  WinLocalSystemSid,
  WinLocalServiceSid,
  WinNetworkServiceSid,
  WinBuiltinDomainSid,
  WinBuiltinAdministratorsSid,
  WinBuiltinUsersSid,
  WinBuiltinGuestsSid,
  WinBuiltinPowerUsersSid,
  WinBuiltinAccountOperatorsSid,
  WinBuiltinSystemOperatorsSid,
  WinBuiltinPrintOperatorsSid,
  WinBuiltinBackupOperatorsSid,
  WinBuiltinReplicatorSid,
  WinBuiltinPreWindows2000CompatibleAccessSid,
  WinBuiltinRemoteDesktopUsersSid,
  WinBuiltinNetworkConfigurationOperatorsSid,
  WinAccountAdministratorSid,
  WinAccountGuestSid,
  WinAccountKrbtgtSid,
  WinAccountDomainAdminsSid,
  WinAccountDomainUsersSid,
  WinAccountDomainGuestsSid,
  WinAccountComputersSid,
  WinAccountControllersSid,
  WinAccountCertAdminsSid,
  WinAccountSchemaAdminsSid,
  WinAccountEnterpriseAdminsSid,
  WinAccountPolicyAdminsSid,
  WinAccountRasAndIasServersSid,
  WinNTLMAuthenticationSid,
  WinDigestAuthenticationSid,
  WinSChannelAuthenticationSid,
  WinThisOrganizationSid,
  WinOtherOrganizationSid,
  WinBuiltinIncomingForestTrustBuildersSid,
  WinBuiltinPerfMonitoringUsersSid,
  WinBuiltinPerfLoggingUsersSid,
  WinBuiltinAuthorizationAccessSid,
  WinBuiltinTerminalServerLicenseServersSid,
  WinBuiltinDCOMUsersSid,
  WinBuiltinIUsersSid,
  WinIUserSid,
  WinBuiltinCryptoOperatorsSid,
  WinUntrustedLabelSid,
  WinLowLabelSid,
  WinMediumLabelSid,
  WinHighLabelSid,
  WinSystemLabelSid,
  WinWriteRestrictedCodeSid,
  WinCreatorOwnerRightsSid,
  WinCacheablePrincipalsGroupSid,
  WinNonCacheablePrincipalsGroupSid,
  WinEnterpriseReadonlyControllersSid,
  WinAccountReadonlyControllersSid,
  WinBuiltinEventLogReadersGroup,
  WinNewEnterpriseReadonlyControllersSid,
  WinBuiltinCertSvcDComAccessGroup,
  WinMediumPlusLabelSid,
  WinLocalLogonSid,
  WinConsoleLogonSid,
  WinThisOrganizationCertificateSid,
  WinApplicationPackageAuthoritySid,
  WinBuiltinAnyPackageSid,
  WinCapabilityInternetClientSid,
  WinCapabilityInternetClientServerSid,
  WinCapabilityPrivateNetworkClientServerSid,
  WinCapabilityPicturesLibrarySid,
  WinCapabilityVideosLibrarySid,
  WinCapabilityMusicLibrarySid,
  WinCapabilityDocumentsLibrarySid,
  WinCapabilitySharedUserCertificatesSid,
  WinCapabilityEnterpriseAuthenticationSid,
  WinCapabilityRemovableStorageSid,
  WinBuiltinRDSRemoteAccessServersSid,
  WinBuiltinRDSEndpointServersSid,
  WinBuiltinRDSManagementServersSid,
  WinUserModeDriversSid,
  WinBuiltinHyperVAdminsSid,
  WinAccountCloneableControllersSid,
  WinBuiltinAccessControlAssistanceOperatorsSid,
  WinBuiltinRemoteManagementUsersSid,
  WinAuthenticationAuthorityAssertedSid,
  WinAuthenticationServiceAssertedSid,
  WinLocalAccountSid,
  WinLocalAccountAndAdministratorSid,
  WinAccountProtectedUsersSid,
  WinCapabilityAppointmentsSid,
  WinCapabilityContactsSid,
  WinAccountDefaultSystemManagedSid,
  WinBuiltinDefaultSystemManagedGroupSid,
  WinBuiltinStorageReplicaAdminsSid,
  WinAccountKeyAdminsSid,
  WinAccountEnterpriseKeyAdminsSid,
  WinAuthenticationKeyTrustSid,
  WinAuthenticationKeyPropertyMFASid,
  WinAuthenticationKeyPropertyAttestationSid,
  WinAuthenticationFreshKeyAuthSid,
  WinBuiltinDeviceOwnersSid,
  WinWellKnownSidTypeCount
} WELL_KNOWN_SID_TYPE;

/* accctrl.h */

typedef enum _SE_OBJECT_TYPE {
	SE_UNKNOWN_OBJECT_TYPE = 0,
	SE_FILE_OBJECT,
	SE_SERVICE,
	SE_PRINTER,
	SE_REGISTRY_KEY,
	SE_LMSHARE,
	SE_KERNEL_OBJECT,
	SE_WINDOW_OBJECT,
	SE_DS_OBJECT,
	SE_DS_OBJECT_ALL,
	SE_PROVIDER_DEFINED_OBJECT,
	SE_WMIGUID_OBJECT,
	SE_REGISTRY_WOW64_32KEY
} SE_OBJECT_TYPE;

WINADVAPI BOOL WINAPI CreateWellKnownSid(WELL_KNOWN_SID_TYPE WellKnownSidType, PSID DomainSid, PSID pSid, DWORD *cbSid);
