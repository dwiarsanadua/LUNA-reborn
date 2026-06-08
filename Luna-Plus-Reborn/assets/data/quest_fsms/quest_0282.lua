--[[
  Quest 282: [Emblem of Knowledge] Dark Omens
  Level Required: 70
  NPC Start: 9, NPC Complete: 24
  Prerequisites: [281]
  Rewards: EXP=651000, Gold=10000
]]

local fsm = require('fsm_engine')

local quest_282 = fsm:new({
    id = 282,
    name = "[Emblem of Knowledge] Dark Omens",
    level_required = 70,
    npc_start = 9,
    npc_complete = 24,
    prerequisites = {281},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 282: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 282: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 282: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 282: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 282: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 282: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 282: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 282: Rewards given")
                self:give_rewards({
                    exp = 651000,
                    gold = 10000,
                    items = {
                        { item_id = 192, count = 1 },
                        { item_id = 192, count = 1 },
                        { item_id = 30000484, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_282