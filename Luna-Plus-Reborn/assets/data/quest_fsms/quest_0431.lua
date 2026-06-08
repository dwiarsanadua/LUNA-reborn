--[[
  Quest 431: [Party][Quest]The infiltration of robbers
  Level Required: 85
  NPC Start: 99, NPC Complete: 99
  Prerequisites: [430]
  Rewards: EXP=3227772
]]

local fsm = require('fsm_engine')

local quest_431 = fsm:new({
    id = 431,
    name = "[Party][Quest]The infiltration of robbers",
    level_required = 85,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {430},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 431: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 431: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 431: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 431: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 284,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 431: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 431: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 431: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 431: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 431: Rewards given")
                self:give_rewards({
                    exp = 3227772,
                    items = {
                        { item_id = 30000256, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_431