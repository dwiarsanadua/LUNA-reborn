--[[
  Quest 587: [Quest] Slime Catch
  Level Required: 0
  NPC Start: 300, NPC Complete: 300
  Prerequisites: [586]
]]

local fsm = require('fsm_engine')

local quest_587 = fsm:new({
    id = 587,
    name = "[Quest] Slime Catch",
    level_required = 0,
    npc_start = 300,
    npc_complete = 300,
    prerequisites = {586},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 587: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 587: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 587: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 3,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 587: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 587: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 587: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 587: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 587: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001098, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_587