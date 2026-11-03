Proposal Procedures usage
=========================

Governance proposals
--------------------

``InfoAction`` is an informational governance proposal. Its CBOR is the
ledger action array ``[6]``::

   const Cometa::InfoAction action;
   const std::vector<std::uint8_t> cbor = action.toCbor();

``NoConfidenceAction`` accepts an optional previous action identifier::

   const Cometa::NoConfidenceAction noConfidence(std::nullopt);
   const std::vector<std::uint8_t> cbor = noConfidence.toCbor();

``HardForkInitiationAction`` combines a proposed protocol version with an
optional previous action identifier::

   const Cometa::HardForkInitiationAction hardFork(
     Cometa::ProtocolVersion(12, 0));

``ParameterChangeAction`` proposes protocol parameter changes. The previous
action identifier and policy hash are optional::

   const Cometa::ParameterChangeAction change(update);
   const std::vector<std::uint8_t> cbor = change.toCbor();

``Constitution`` holds its anchor and an optional script hash::

   const Cometa::Constitution constitution(anchor, std::nullopt);

``NewConstitutionAction`` proposes a constitution and may refer to the
previous constitutional action::

   const Cometa::NewConstitutionAction newConstitution(constitution);

``TreasuryWithdrawalsAction`` holds a withdrawal map and optional policy hash::

   const Cometa::TreasuryWithdrawalsAction treasury(withdrawals);

``CommitteeMembersMap`` associates committee credentials with their term
expiration epochs::

   Cometa::CommitteeMembersMap members;
   const Cometa::Credential coldCredential(
     Cometa::CredentialType::KeyHash,
     Cometa::Blake2bHash::fromHex(std::string(56, '0')));
   members.insert(coldCredential, 200);
   const std::optional<std::uint64_t> expiration = members.get(coldCredential);

``Committee`` adds a quorum threshold to the member terms::

   Cometa::Committee committee(Cometa::UnitInterval(1, 2));
   committee.addMember(coldCredential, 200);
   const Cometa::UnitInterval quorum = committee.quorumThreshold();

``UpdateCommitteeAction`` proposes members to remove or add and a new quorum::

   const Cometa::UpdateCommitteeAction updateCommittee(
     Cometa::CredentialSet(), members, Cometa::UnitInterval(1, 2));
   const std::vector<std::uint8_t> proposalCbor = updateCommittee.toCbor();

``ProposalProcedure`` pairs one governance action with its deposit, return
address, and metadata anchor::

   const Cometa::ProposalProcedure proposal(
     500, Cometa::RewardAddress(Cometa::NetworkId::Testnet, coldCredential),
     Cometa::Anchor("https://a", Cometa::Blake2bHash::fromHex(std::string(64, '0'))),
     Cometa::InfoAction());
   const Cometa::InfoAction action = proposal.asInfoAction();

``ProposalProcedureSet`` stores proposals in insertion order::

   Cometa::ProposalProcedureSet proposals;
   proposals.add(proposal);
   const Cometa::ProposalProcedure first = proposals.at(0);
