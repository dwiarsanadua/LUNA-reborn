--[[
  Quest 588: [Quest] Fungus Pawn catching
  Level Required: 0
  NPC Start: 300, NPC Complete: 300
  Prerequisites: [587]
]]

local fsm = require('fsm_engine')

local quest_588 = fsm:new({
    id = 588,
    name = "[Quest] Fungus Pawn catching",
    level_required = 0,
    npc_start = 300,
    npc_complete = 300,
    prerequisites = {587},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 588: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 588: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 588: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 18,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 588: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 588: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 588: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 588: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 588: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001096, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_588