--[[
  Quest 836: Evidence of Replication 2
  Level Required: 79
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [833]
  Rewards: EXP=2627089, Gold=61514
]]

local fsm = require('fsm_engine')

local quest_836 = fsm:new({
    id = 836,
    name = "Evidence of Replication 2",
    level_required = 79,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {833},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 836: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 836: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 836: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 74,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 836: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 836: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 836: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 836: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 836: Rewards given")
                self:give_rewards({
                    exp = 2627089,
                    gold = 61514,
                    items = {
                        { item_id = 391, count = 1 },
                        { item_id = 391, count = 40 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_836