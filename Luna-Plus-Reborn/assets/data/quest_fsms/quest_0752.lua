--[[
  Quest 752: [Quest]Home Sweet Home
  Level Required: 2
  NPC Start: 49, NPC Complete: 200
  Rewards: EXP=12
]]

local fsm = require('fsm_engine')

local quest_752 = fsm:new({
    id = 752,
    name = "[Quest]Home Sweet Home",
    level_required = 2,
    npc_start = 49,
    npc_complete = 200,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 752: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 752: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 752: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 752: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 200,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 752: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 752: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 200,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 752: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 752: Rewards given")
                self:give_rewards({
                    exp = 12,
                    items = {
                        { item_id = 21001517, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_752