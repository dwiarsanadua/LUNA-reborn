--[[
  Quest 429: [Party][Quest]Contact with The Black Widow party
  Level Required: 84
  NPC Start: 99, NPC Complete: 99
  Prerequisites: [428]
  Rewards: EXP=2964862
]]

local fsm = require('fsm_engine')

local quest_429 = fsm:new({
    id = 429,
    name = "[Party][Quest]Contact with The Black Widow party",
    level_required = 84,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {428},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 429: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 429: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 429: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 278,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 429: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 429: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 429: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 429: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 429: Rewards given")
                self:give_rewards({
                    exp = 2964862,
                    items = {
                        { item_id = 30000208, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_429