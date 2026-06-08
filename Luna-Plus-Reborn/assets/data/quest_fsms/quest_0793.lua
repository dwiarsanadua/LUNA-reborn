--[[
  Quest 793: Real or Fake
  Level Required: 62
  NPC Start: 44, NPC Complete: 44
  Rewards: EXP=475228, Gold=14506
]]

local fsm = require('fsm_engine')

local quest_793 = fsm:new({
    id = 793,
    name = "Real or Fake",
    level_required = 62,
    npc_start = 44,
    npc_complete = 44,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 793: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 793: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 793: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 793: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 793: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 793: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 44,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 793: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 793: Rewards given")
                self:give_rewards({
                    exp = 475228,
                    gold = 14506,
                    items = {
                        { item_id = 21000271, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_793