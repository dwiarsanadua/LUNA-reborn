--[[
  Quest 355: [Party] Shadow in Darkness
  Level Required: 38
  NPC Start: 59, NPC Complete: 59
  Prerequisites: [354]
  Rewards: EXP=96964, Gold=9000
]]

local fsm = require('fsm_engine')

local quest_355 = fsm:new({
    id = 355,
    name = "[Party] Shadow in Darkness",
    level_required = 38,
    npc_start = 59,
    npc_complete = 59,
    prerequisites = {354},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 355: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 355: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 355: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 159,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 355: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 355: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 355: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 59,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 355: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 355: Rewards given")
                self:give_rewards({
                    exp = 96964,
                    gold = 9000,
                    items = {
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_355