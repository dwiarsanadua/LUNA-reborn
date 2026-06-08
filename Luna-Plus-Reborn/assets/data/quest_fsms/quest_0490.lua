--[[
  Quest 490: [Special][Quest] Black Gargoyle
  Level Required: 34
  NPC Start: 28, NPC Complete: 28
  Rewards: EXP=147519, Gold=113050
]]

local fsm = require('fsm_engine')

local quest_490 = fsm:new({
    id = 490,
    name = "[Special][Quest] Black Gargoyle",
    level_required = 34,
    npc_start = 28,
    npc_complete = 28,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 490: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 490: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 490: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 83,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 490: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 490: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 490: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 490: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 490: Rewards given")
                self:give_rewards({
                    exp = 147519,
                    gold = 113050,
                    items = {
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_490