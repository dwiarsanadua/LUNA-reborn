--[[
  Quest 440: [Repeatable][Quest] Collecting war trophies
  Level Required: 55
  NPC Start: 118, NPC Complete: 118
  Prerequisites: [439]
  Rewards: EXP=361052
]]

local fsm = require('fsm_engine')

local quest_440 = fsm:new({
    id = 440,
    name = "[Repeatable][Quest] Collecting war trophies",
    level_required = 55,
    npc_start = 118,
    npc_complete = 118,
    prerequisites = {439},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 440: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 440: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 440: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 286,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 440: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 440: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 98,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 440: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 440: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 440: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 440: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 440: Rewards given")
                self:give_rewards({
                    exp = 361052,
                    items = {
                        { item_id = 316, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_440