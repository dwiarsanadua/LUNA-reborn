--[[
  Quest 860: He's Happy
  Level Required: 89
  NPC Start: 126, NPC Complete: 126
  Rewards: EXP=4431094, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_860 = fsm:new({
    id = 860,
    name = "He's Happy",
    level_required = 89,
    npc_start = 126,
    npc_complete = 126,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 860: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 860: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 860: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 860: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 860: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 860: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 860: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 860: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 860: Rewards given")
                self:give_rewards({
                    exp = 4431094,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_860