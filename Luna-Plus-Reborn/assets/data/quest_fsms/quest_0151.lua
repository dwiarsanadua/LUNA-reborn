--[[
  Quest 151: Protecting the Outpost
  Level Required: 1
  NPC Start: 13, NPC Complete: 15
  Prerequisites: [85]
  Rewards: EXP=964, Gold=795
]]

local fsm = require('fsm_engine')

local quest_151 = fsm:new({
    id = 151,
    name = "Protecting the Outpost",
    level_required = 1,
    npc_start = 13,
    npc_complete = 15,
    prerequisites = {85},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 151: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 151: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 151: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 151: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 151: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 151: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 151: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 151: Rewards given")
                self:give_rewards({
                    exp = 964,
                    gold = 795,
                    items = {
                        { item_id = 68, count = 1 },
                        { item_id = 21000006, count = 6 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_151