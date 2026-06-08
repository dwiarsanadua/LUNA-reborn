--[[
  Quest 989: Quest 989
  Level Required: 150
  NPC Start: 302, NPC Complete: 303
]]

local fsm = require('fsm_engine')

local quest_989 = fsm:new({
    id = 989,
    name = "Quest 989",
    level_required = 150,
    npc_start = 302,
    npc_complete = 303,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 989: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 302,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 989: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 989: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 302,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 989: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 303,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 989: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 989: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 303,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 989: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 989: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000613, count = 20 },
                        { item_id = 21001125, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_989