--[[
  Quest 126: Tarintus' Resurrection
  Level Required: 25
  NPC Start: 9, NPC Complete: 27
  Prerequisites: [125]
  Rewards: EXP=8697, Gold=610
]]

local fsm = require('fsm_engine')

local quest_126 = fsm:new({
    id = 126,
    name = "Tarintus' Resurrection",
    level_required = 25,
    npc_start = 9,
    npc_complete = 27,
    prerequisites = {125},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 126: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 126: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 126: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 126: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 126: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 126: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 126: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 126: Rewards given")
                self:give_rewards({
                    exp = 8697,
                    gold = 610,
                    items = {
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_126