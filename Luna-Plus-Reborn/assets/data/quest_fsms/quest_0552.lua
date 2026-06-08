--[[
  Quest 552: [Repeat][Quest] Tainted Dream World
  Level Required: 1
  NPC Start: 142, NPC Complete: 142
]]

local fsm = require('fsm_engine')

local quest_552 = fsm:new({
    id = 552,
    name = "[Repeat][Quest] Tainted Dream World",
    level_required = 1,
    npc_start = 142,
    npc_complete = 142,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 552: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 142,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 552: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 552: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 418,
                    count = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 552: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 142,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 552: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 552: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 142,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 552: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 552: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000625, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_552