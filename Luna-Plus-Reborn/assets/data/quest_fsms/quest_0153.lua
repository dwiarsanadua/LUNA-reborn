--[[
  Quest 153: Journey to Tarintus
  Level Required: 22
  NPC Start: 25, NPC Complete: 57
  Prerequisites: [152, 106, 108]
  Rewards: EXP=5939, Gold=530
]]

local fsm = require('fsm_engine')

local quest_153 = fsm:new({
    id = 153,
    name = "Journey to Tarintus",
    level_required = 22,
    npc_start = 25,
    npc_complete = 57,
    prerequisites = {152, 106, 108},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 153: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 153: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 153: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 153: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 153: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 153: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 153: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 153: Rewards given")
                self:give_rewards({
                    exp = 5939,
                    gold = 530,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_153