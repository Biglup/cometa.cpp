Voting Procedures usage
=======================

Governance votes
----------------

``Vote`` and ``VoterType`` identify governance choices and voter credentials::

   const Cometa::Vote choice = Cometa::Vote::Yes;
   const Cometa::VoterType voter = Cometa::VoterType::DRepKeyHash;

``Voter`` combines a voter kind with its credential::

   const Cometa::Voter drepVoter(Cometa::VoterType::DRepKeyHash, coldCredential);

Voting IDs, voters, and procedures can be stored in ``GovernanceActionIdList``, ``VoterList``, and ``VotingProcedureList`` with the usual ``std::vector`` operations.

``VotingProcedures`` stores votes by voter and governance action. Its ``insert``, ``get``, ``voters``, and ``actionIds`` methods work with independent C++ values.

``VotingProcedure`` records a choice and an optional metadata anchor::

   const Cometa::VotingProcedure castVote(Cometa::Vote::Yes);
