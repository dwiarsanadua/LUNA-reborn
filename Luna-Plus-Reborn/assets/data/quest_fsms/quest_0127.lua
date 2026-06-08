--[[
  Quest 127: Rigorous Training
  Level Required: 25
  NPC Start: 9, NPC Complete: 29
  Prerequisites: [126]
  Rewards: EXP=8697, Gold=640
]]

local fsm = require('fsm_engine')

local quest_127 = fsm:new({
    id = 127,
    name = "Rigorous Training",
    level_required = 25,
    npc_start = 9,
    npc_complete = 29,
    prerequisites = {126},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 127: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 127: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 127: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 127: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 127: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 127: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 127: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 127: Rewards given")
                self:give_rewards({
                    exp = 8697,
                    gold = 640,
                    items = {
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_127